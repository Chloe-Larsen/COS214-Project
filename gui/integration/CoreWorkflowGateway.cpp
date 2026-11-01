#include "CoreWorkflowGateway.h"
#include "GuiOutputAdapter.h"
#include "services/JsonCodec.h"
#include "chain/Worker.h"
#include "composite/Item.h"
#include "composite/ItemGroup.h"
#include "command/Assign.h"
#include "command/Cancel.h"
#include "command/Escalate.h"
#include "command/Progress.h"
#include "command/Reject.h"
#include "state/Available.h"
#include "state/Blocked.h"
#include "state/InProgress.h"
#include "state/Approved.h"
#include "state/Completed.h"
#include "state/Cancelled.h"
#include "strategy/AssignmentStrategy.h"
#include "adapter/EmailServiceAdapter.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <limits>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace tividy {
std::string operationName(Operation op)
{
    switch (op) {
    case Operation::CreateWorkflow: return "New workflow";
    case Operation::AddItem: return "Add item";
    case Operation::AddGroup: return "Add group";
    case Operation::RenameComponent: return "Rename";
    case Operation::SetPriority: return "Set priority";
    case Operation::Assign: return "Assign worker";
    case Operation::Unassign: return "Unassign worker";
    case Operation::Progress: return "Progress";
    case Operation::Review: return "Request review";
    case Operation::Reject: return "Reject";
    case Operation::Escalate: return "Escalate";
    case Operation::Cancel: return "Cancel work";
    case Operation::AddWorker: return "Add worker";
    case Operation::EditWorker: return "Edit worker";
    case Operation::RemoveWorker: return "Remove worker";
    case Operation::SetSupervisor: return "Set superior";
    case Operation::SetStrategy: return "Assignment strategy";
    case Operation::AddDependency: return "Add dependency";
    case Operation::RemoveDependency: return "Remove dependency";
    case Operation::ConfigureNotifier: return "Configure notifier";
    case Operation::ConfigureAudit: return "Configure audit logger";
    }
    throw std::invalid_argument("Unknown operation.");
}

namespace {
typedef std::map<std::string, std::string> Options;
struct InputError : std::runtime_error {
    std::string path;
    InputError(std::string field, const std::string &message)
        : std::runtime_error(message), path(std::move(field)) {}
};
void require(bool condition, const std::string &path, const std::string &message)
{
    if (!condition) throw InputError(path, message);
}
std::string option(const Options &settings, const std::string &name, const std::string &fallback = "")
{
    const auto it = settings.find(name);
    return it == settings.end() ? fallback : it->second;
}
std::string timestamp()
{
    static std::mutex lock;
    const std::time_t now = std::time(nullptr);
    std::tm utc = {};
    {
        std::lock_guard<std::mutex> guard(lock);
        const std::tm *value = std::gmtime(&now);
        if (!value) throw std::runtime_error("The current time could not be read.");
        utc = *value;
    }
    std::ostringstream out;
    out << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}
std::unique_ptr<State> savedState(const std::string &name)
{
    if (name.empty() || name == "AVAILABLE") return std::unique_ptr<State>(new Available());
    if (name == "BLOCKED") return std::unique_ptr<State>(new Available()); // Readiness sets Blocked below.
    if (name == "IN_PROGRESS") return std::unique_ptr<State>(new InProgress());
    if (name == "APPROVED") return std::unique_ptr<State>(new Approved());
    if (name == "COMPLETED") return std::unique_ptr<State>(new Completed());
    if (name == "CANCELLED") return std::unique_ptr<State>(new Cancelled());
    throw InputError("/state", "Unknown item state: " + name);
}

/**
 * GUI-facing Item using the existing virtual lifecycle interface.
 * Calling the State once avoids the supplied Item::progress double transition;
 * explicit review uses the existing Worker chain with one required approval.
 */
class GuiItem : public Item {
public:
    /** Construct through the fully initialised core constructor. */
    GuiItem(const ComponentRecord &c, Worker *worker)
        : Item(c.name, c.deadline, c.priority, worker) {}
    /** Execute one State transition through a Progress command. */
    void progress() override {
        require(getDependencies() == 0, "/dependencies", "This item has unfinished prerequisites.");
        const std::string status = getStatus();
        require(status == "AVAILABLE" || status == "APPROVED", "/state",
                "Start available work, request review, then complete approved work.");
        getState()->progress(this);
    }
    /** Ask the assigned worker's actual superior to review this item. */
    void requestReview() override {
        require(getStatus() == "IN_PROGRESS", "/state", "Start the item before requesting review.");
        Worker *reviewer = getWorker() ? getWorker()->getSupervisor() : nullptr;
        require(reviewer != nullptr, "/worker", "The assignee needs a superior to review this item.");
        if (reviewer->review(this, 1)) {
            std::unique_ptr<State> approved(new Approved());
            setState(approved.get()); approved.release();
        }
    }
};

/** Common configuration for GUI policies extending the core Strategy interface. */
class GuiAssignment : public AssignmentStrategy {
protected:
    std::vector<Worker *> people;
    std::vector<Item *> items;
    Options settings;
    std::vector<Worker *> candidates() const {
        std::vector<Worker *> result;
        const std::string role = option(settings, "role");
        for (Worker *w : people)
            if (role.empty() || w->getRole() == role) result.push_back(w);
        const bool byName = option(settings, "tieBreak", "workerId") == "workerName";
        std::sort(result.begin(), result.end(), [byName](Worker *a, Worker *b) {
            if (byName && a->getName() != b->getName()) return a->getName() < b->getName();
            return a->getId() < b->getId();
        });
        return result;
    }
    Worker *leastBusy() const {
        Worker *best = nullptr;
        std::size_t least = std::numeric_limits<std::size_t>::max();
        for (Worker *w : candidates()) {
            std::size_t count = 0;
            for (Item *item : items)
                if (item->getWorker() == w && item->getStatus() != "COMPLETED" && item->getStatus() != "CANCELLED") ++count;
            if (count < least) { best = w; least = count; }
        }
        return best;
    }
public:
    /** Borrow workers/items for one operation on the serial engine queue. */
    GuiAssignment(std::vector<Worker *> workers, std::vector<Item *> work, Options configuration)
        : people(std::move(workers)), items(std::move(work)), settings(std::move(configuration)) {}
};
/** Select the least busy worker with the requested role. */
class GuiRoleBased : public GuiAssignment {
public:
    using GuiAssignment::GuiAssignment;
    /** Select a worker; the caller performs the real Assign command. */
    Worker *assign(Item *, WorkflowManager *) override {
        return option(settings, "role").empty() ? nullptr : leastBusy();
    }
};
/** Select an eligible worker with the least active assigned work. */
class GuiWorkloadBased : public GuiAssignment {
public:
    using GuiAssignment::GuiAssignment;
    /** Count live core items without invoking the unsafe legacy iterator. */
    Worker *assign(Item *, WorkflowManager *) override {
        return leastBusy();
    }
};
OperationResult failed(const std::string &path, const std::string &message)
{
    OperationResult result; result.message = "The workflow was not changed.";
    Diagnostic d; d.location = path; d.message = message; result.diagnostics.push_back(d);
    return result;
}
const std::set<std::string> eventTypes = {
    "ASSIGNED", "IN_PROGRESS", "COMPLETED", "APPROVED", "REJECTED", "ESCALATED", "CANCELLED", "UPDATED"
};
void validateObserver(const Options &settings, bool notifier)
{
    if (settings.empty()) return;
    for (const auto &value : settings)
        require(value.first == "enabled" || value.first == "events" || value.first == "scope" ||
                value.first == "recipients" || value.first == "adapter", "/observers", "Unknown observer setting: " + value.first);
    for (const std::string name : {"enabled", "adapter"})
        require(!settings.count(name) || option(settings, name) == "true" || option(settings, name) == "false",
                "/observers/" + name, "Use true or false.");
    const std::string scope = option(settings, "scope", "component");
    require(scope == "workflow" || scope == "component", "/observers/scope", "Choose a component or workflow scope.");
    if (notifier) {
        const std::string recipients = option(settings, "recipients", "assignee");
        require(recipients == "assignee" || recipients == "superior" || recipients == "both",
                "/notifier/recipients", "Choose assignee, superior or both.");
    }
    std::istringstream list(option(settings, "events"));
    std::string type; bool any = false;
    while (std::getline(list, type, ',')) {
        require(eventTypes.count(type) != 0, "/observers/events", "Unknown event: " + type); any = true;
    }
    require(option(settings, "enabled") != "true" || any, "/observers/events", "Choose at least one event.");
}
bool watches(const Options &settings, const std::string &type)
{
    return option(settings, "enabled") == "true" &&
        ("," + option(settings, "events") + ",").find("," + type + ",") != std::string::npos;
}
}

/** One owned session; views never receive any pointer stored here. */
struct CoreWorkflowGateway::Storage {
    struct WorkerPool {
        std::map<std::string, std::unique_ptr<Worker> > values;
        ~WorkerPool() {
            for (auto &entry : values) {
                entry.second->setWorkflowRoot(nullptr);
                entry.second->setSupervisor(nullptr);
            }
        }
    };
    WorkflowDocument document;
    WorkerPool people; // Declared before root: root is destroyed first.
    std::unique_ptr<ItemGroup> root;
    std::map<std::string, WorkComponent *> components;
    std::map<std::string, std::size_t> indexes, workerIndexes;
    std::map<std::string, std::vector<std::size_t> > children;
    std::vector<std::size_t> order;
    std::set<std::string> activityIds;
    std::uint64_t sequence;

    explicit Storage(WorkflowDocument value, bool strict = false)
        : document(std::move(value)), root(new ItemGroup(document.data.name)), sequence(0)
    {
        // Reuse the transport validator for direct gateway callers as well as files.
        JsonCodec::encode(document);
        WorkflowSnapshot &s = document.data;
        require(s.assignmentStrategy.empty() || s.assignmentStrategy == "RoleBased" || s.assignmentStrategy == "WorkloadBased",
                "/assignmentStrategy", "Choose RoleBased or WorkloadBased.");
        require(s.assignmentStrategy != "RoleBased" || !option(s.strategyOptions, "role").empty(),
                "/strategyOptions/role", "Choose a role for role-based assignment.");
        for (const auto &setting : s.strategyOptions)
            require(setting.first == "role" || setting.first == "tieBreak", "/strategyOptions", "Unknown strategy setting: " + setting.first);
        require(option(s.strategyOptions, "tieBreak", "workerId") == "workerId" ||
                option(s.strategyOptions, "tieBreak") == "workerName", "/strategyOptions/tieBreak", "Choose workerId or workerName.");
        for (std::size_t i = 0; i < s.workers.size(); ++i) workerIndexes.emplace(s.workers[i].id, i);
        for (const WorkerRecord &w : s.workers) {
            std::set<std::string> chain;
            std::string cursor = w.id;
            while (!cursor.empty()) {
                require(chain.insert(cursor).second, "/workers/" + w.id + "/supervisorId", "This superior relationship creates a circular hierarchy.");
                require(chain.size() <= 64, "/workers", "A reporting hierarchy may have at most 64 levels.");
                cursor = s.workers.at(workerIndexes.at(cursor)).supervisorId;
            }
            require(w.email.empty() || (w.email.find('@') != std::string::npos &&
                    w.email.find_first_of(" \t\r\n") == std::string::npos), "/workers/" + w.id + "/email", "Enter a valid email address.");
            people.values.emplace(w.id, std::unique_ptr<Worker>(new Worker(w.id, w.name, w.role)));
        }
        for (const WorkerRecord &w : s.workers) {
            Worker *person = worker(w.id);
            person->setWorkflowRoot(root.get());
            if (!w.supervisorId.empty()) person->setSupervisor(worker(w.supervisorId));
        }
        for (std::size_t i = 0; i < s.components.size(); ++i) {
            indexes.emplace(s.components[i].id, i);
            children[s.components[i].parentId].push_back(i);
            validateObserver(s.components[i].notifier, true); validateObserver(s.components[i].audit, false);
        }
        validateObserver(workflowSettings("notifier"), true); validateObserver(workflowSettings("audit"), false);
        std::map<std::string, int> depth;
        order = children[""];
        for (std::size_t offset = 0; offset < order.size(); ++offset) {
            const std::size_t i = order[offset];
            const ComponentRecord &c = s.components[i];
            if (!c.parentId.empty())
                require(s.components.at(indexes.at(c.parentId)).group, "/components/" + c.id + "/parentId", "A parent must be a group.");
            require(depth[c.id] <= 256, "/components", "The hierarchy may have at most 256 levels.");
            for (std::size_t child : children[c.id]) {
                depth[s.components[child].id] = depth[c.id] + 1; order.push_back(child);
            }
        }
        require(order.size() == s.components.size(), "/components", "The item hierarchy contains a cycle.");
        for (std::size_t i : order) {
            const ComponentRecord &c = s.components[i];
            std::unique_ptr<WorkComponent> node;
            if (c.group) {
                require(c.workerId.empty() && c.state.empty() && c.deadline.empty() && !c.rejected && !c.escalated,
                        "/components/" + c.id, "Group states and assignees are derived from their items.");
                node.reset(new ItemGroup(c.name));
            } else {
                require(children[c.id].empty(), "/components/" + c.id, "An item cannot have children.");
                std::unique_ptr<GuiItem> item(new GuiItem(c, c.workerId.empty() ? nullptr : worker(c.workerId)));
                std::unique_ptr<State> initial = savedState(c.state);
                item->setState(initial.get()); initial.release();
                if (c.rejected) item->reject(c.rejectionReason);
                if (c.escalated) {
                    item->getState()->setEscalated();
                    require(item->isEscalated(), "/components/" + c.id + "/escalated", "This state does not support escalation.");
                }
                node = std::move(item);
            }
            ItemGroup *parent = c.parentId.empty() ? root.get() : dynamic_cast<ItemGroup *>(component(c.parentId));
            parent->add(node.get()); components.emplace(c.id, node.get()); node.release();
        }
        validateDependencies();
        syncDependencies();
        if (strict) for (const ComponentRecord &c : s.components)
            if (!c.group && !c.state.empty())
                require(c.state == dynamic_cast<Item *>(component(c.id))->getStatus(), "/components/" + c.id + "/state",
                        "The saved state disagrees with dependency readiness.");
        const std::string saved = option(document.engineData, "activitySequence", "0");
        require(!saved.empty() && saved.find_first_not_of("0123456789") == std::string::npos,
                "/engineData/activitySequence", "Expected an unsigned activity sequence.");
        try { sequence = std::stoull(saved); }
        catch (...) { throw InputError("/engineData/activitySequence", "The activity sequence is out of range."); }
        require(sequence < std::numeric_limits<std::uint64_t>::max() - 1000000, "/engineData/activitySequence", "The activity sequence is exhausted.");
        for (const ActivityRecord &a : s.activity)
            require(activityIds.insert(a.id).second, "/activity", "Activity IDs must be unique.");
        refresh();
    }
    Worker *worker(const std::string &id) const {
        const auto it = people.values.find(id);
        require(it != people.values.end(), "/workerId", "Unknown worker: " + id);
        return it->second.get();
    }
    WorkComponent *component(const std::string &id) const {
        const auto it = components.find(id);
        require(it != components.end(), "/componentId", "Unknown component: " + id);
        return it->second;
    }
    ComponentRecord &record(const std::string &id) { return document.data.components.at(indexes.at(id)); }
    Options workflowSettings(const std::string &kind) const {
        Options result;
        const std::string prefix = kind + ".";
        for (const auto &entry : document.engineData)
            if (entry.first.compare(0, prefix.size(), prefix) == 0) result[entry.first.substr(prefix.size())] = entry.second;
        return result;
    }
    Options settings(const std::string &kind, std::string id) const {
        while (!id.empty()) {
            const ComponentRecord &c = document.data.components.at(indexes.at(id));
            const Options &local = kind == "notifier" ? c.notifier : c.audit;
            if (!local.empty()) return local;
            id = c.parentId;
        }
        return workflowSettings(kind);
    }
    void validateDependencies() const {
        const std::size_t count = document.data.components.size();
        std::vector<std::vector<std::size_t> > edges(count * 2);
        std::vector<int> incoming(count * 2, 0);
        const auto link = [&](std::size_t a, std::size_t b) { edges[a].push_back(b); ++incoming[b]; };
        for (std::size_t i = 0; i < count; ++i) {
            const ComponentRecord &c = document.data.components[i];
            const auto list = children.find(c.id);
            if (!c.group || list == children.end() || list->second.empty()) link(i * 2, i * 2 + 1);
            else for (std::size_t child : list->second) { link(i * 2, child * 2); link(child * 2 + 1, i * 2 + 1); }
        }
        // Start/finish barriers include group membership in cycle detection.
        // No Cartesian expansion of all descendant leaf pairs is needed.
        for (const DependencyRecord &d : document.data.dependencies)
            link(indexes.at(d.prerequisiteId) * 2 + 1, indexes.at(d.dependentId) * 2);
        std::queue<std::size_t> ready;
        for (std::size_t i = 0; i < incoming.size(); ++i) if (incoming[i] == 0) ready.push(i);
        std::size_t visited = 0;
        while (!ready.empty()) {
            const std::size_t i = ready.front(); ready.pop(); ++visited;
            for (std::size_t next : edges[i]) if (--incoming[next] == 0) ready.push(next);
        }
        require(visited == incoming.size(), "/dependencies", "This dependency creates a cycle, including an item/group relationship.");
    }
    void syncDependencies() {
        std::map<std::string, bool> satisfied;
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            const ComponentRecord &c = document.data.components[*it];
            Item *item = dynamic_cast<Item *>(component(c.id));
            if (item) satisfied[c.id] = !item->isRejected() && (item->getStatus() == "APPROVED" || item->getStatus() == "COMPLETED");
            else {
                bool complete = !children[c.id].empty();
                for (std::size_t child : children[c.id]) complete = complete && satisfied[document.data.components[child].id];
                satisfied[c.id] = complete;
            }
        }
        std::map<std::string, int> pending;
        for (const DependencyRecord &d : document.data.dependencies)
            if (!satisfied[d.prerequisiteId]) ++pending[d.dependentId];
        for (std::size_t i : order) {
            const ComponentRecord &c = document.data.components[i];
            if (!c.parentId.empty()) pending[c.id] += pending[c.parentId];
            Item *item = dynamic_cast<Item *>(component(c.id));
            if (!item || item->getStatus() == "CANCELLED") continue;
            require(pending[c.id] == 0 || (item->getStatus() != "APPROVED" && item->getStatus() != "COMPLETED"),
                    "/dependencies", "Finished work cannot acquire an unfinished prerequisite.");
            while (item->getDependencies() < pending[c.id]) item->incrementDependencies();
            while (item->getDependencies() > pending[c.id]) item->decrementDependencies();
        }
    }
    void refresh() {
        std::map<std::string, int> workload;
        for (ComponentRecord &c : document.data.components) {
            c.name = component(c.id)->getName(); c.allowedActions.clear();
            Item *item = dynamic_cast<Item *>(component(c.id));
            if (!item) continue;
            c.state = item->getStatus(); c.priority = item->getPriority();
            c.workerId = item->getWorker() ? item->getWorker()->getId() : "";
            c.rejected = item->isRejected(); c.escalated = item->isEscalated(); c.rejectionReason = item->getRejectionReason();
            if (!c.workerId.empty() && c.state != "COMPLETED" && c.state != "CANCELLED") ++workload[c.workerId];
        }
        for (WorkerRecord &w : document.data.workers) w.workload = workload[w.id];
    }
    WorkflowSnapshot snapshot(std::uint64_t revision) const {
        WorkflowSnapshot s = document.data;
        s.notifier = workflowSettings("notifier"); s.audit = workflowSettings("audit");
        s.revision = revision; s.dirty = revision != 0; s.canImport = s.canExport = true;
        s.availabilityMessage.clear(); s.capabilities.clear();
        for (int i = 0; i <= static_cast<int>(Operation::ConfigureAudit); ++i) s.capabilities.insert(static_cast<Operation>(i));
        for (ComponentRecord &c : s.components) {
            c.allowedActions = {Operation::RenameComponent};
            if (c.group) {
                c.allowedActions.insert(Operation::AddItem); c.allowedActions.insert(Operation::AddGroup);
                for (const std::string &id : leaves(c.id)) {
                    const ComponentRecord &leaf = document.data.components.at(indexes.at(id));
                    if (leaf.state == "COMPLETED" || leaf.state == "CANCELLED") continue;
                    c.allowedActions.insert(Operation::Cancel); c.allowedActions.insert(Operation::Assign);
                    if (!leaf.workerId.empty()) c.allowedActions.insert(Operation::Unassign);
                    if (leaf.state == "AVAILABLE" || leaf.state == "APPROVED") c.allowedActions.insert(Operation::Progress);
                    if (leaf.state == "IN_PROGRESS") c.allowedActions.insert(Operation::Reject);
                    if (!leaf.workerId.empty() && worker(leaf.workerId)->getSupervisor()) {
                        if (leaf.state == "IN_PROGRESS") c.allowedActions.insert(Operation::Review);
                        if (leaf.state != "APPROVED") c.allowedActions.insert(Operation::Escalate);
                    }
                }
                continue;
            }
            c.allowedActions.insert(Operation::SetPriority);
            if (c.state == "CANCELLED" || c.state == "COMPLETED") continue;
            c.allowedActions.insert(Operation::Assign); c.allowedActions.insert(Operation::Cancel);
            if (c.state == "AVAILABLE" || c.state == "APPROVED") c.allowedActions.insert(Operation::Progress);
            if (c.state == "IN_PROGRESS") {
                c.allowedActions.insert(Operation::Reject);
                if (!c.workerId.empty() && worker(c.workerId)->getSupervisor()) c.allowedActions.insert(Operation::Review);
            }
            if (!c.workerId.empty()) {
                c.allowedActions.insert(Operation::Unassign);
                if (c.state != "APPROVED" && worker(c.workerId)->getSupervisor()) c.allowedActions.insert(Operation::Escalate);
            }
        }
        if (s.activity.size() > 10000) s.activity.erase(s.activity.begin(), s.activity.end() - 10000);
        return s;
    }
    void append(const std::string &category, std::string message, const std::string &id = "") {
        ActivityRecord a;
        do { a.id = "activity-" + std::to_string(++sequence); } while (activityIds.count(a.id));
        a.timestamp = timestamp(); a.category = category;
        a.componentId = components.count(id) ? id : "";
        a.componentName = a.componentId.empty() ? document.data.name : component(id)->getName();
        if (message.size() > 65536) {
            std::size_t end = 65536;
            while (end && (static_cast<unsigned char>(message[end]) & 0xc0) == 0x80) --end;
            message.resize(end);
        }
        a.message = std::move(message); activityIds.insert(a.id); document.data.activity.push_back(a);
        if (document.data.activity.size() > 100000) {
            activityIds.erase(document.data.activity.front().id); document.data.activity.erase(document.data.activity.begin());
        }
        document.engineData["activitySequence"] = std::to_string(sequence);
    }
    void publish(const std::string &type, const std::string &id, const std::string &message) {
        if (watches(settings("audit", id), type)) append("Audit", type + " · " + message, id);
        const Options notify = settings("notifier", id);
        if (!watches(notify, type)) return;
        append("Notification", type + " · " + message, id);
        if (option(notify, "adapter") != "true") return;
        Item *item = id.empty() ? nullptr : dynamic_cast<Item *>(component(id));
        Worker *assigned = item ? item->getWorker() : nullptr;
        std::set<std::string> recipients;
        const std::string choice = option(notify, "recipients", "assignee");
        if (assigned && choice != "superior") recipients.insert(assigned->getId());
        if (assigned && assigned->getSupervisor() && choice != "assignee") recipients.insert(assigned->getSupervisor()->getId());
        if (recipients.empty()) { append("Notification", "No eligible recipient for adapter output.", id); return; }
        for (const std::string &recipient : recipients) {
            const WorkerRecord &w = document.data.workers.at(workerIndexes.at(recipient));
            if (w.email.empty()) { append("Notification", "Add an email address for " + w.name + " to enable adapter output.", id); continue; }
            try {
                GuiOutputAdapter adapter(std::unique_ptr<EmailService>(new EmailServiceAdapter()),
                    [this, id](AdapterOutput output) {
                        append("Adapter", "Legacy email adapter · " + output.recipient + "\n" + output.subject + "\n" + output.body, id);
                    });
                adapter.sendEmail(w.email, "TIVIDY · " + type, message);
            } catch (const std::exception &e) {
                append("Adapter", std::string("Adapter output failed: ") + e.what(), id);
            } catch (...) { append("Adapter", "Adapter output failed. The completed workflow action was retained.", id); }
        }
    }
    std::vector<std::string> leaves(const std::string &id) const {
        std::vector<std::string> result, pending(1, id);
        for (std::size_t i = 0; i < pending.size(); ++i) {
            const ComponentRecord &c = document.data.components.at(indexes.at(pending[i]));
            if (!c.group) result.push_back(c.id);
            const auto list = children.find(c.id);
            if (list != children.end()) for (std::size_t child : list->second) pending.push_back(document.data.components[child].id);
        }
        return result;
    }
    std::size_t assignUnassigned(const std::string &only = "") {
        std::vector<Worker *> workers;
        std::vector<Item *> items;
        for (const auto &w : people.values) workers.push_back(w.second.get());
        for (const auto &entry : components)
            if (Item *item = dynamic_cast<Item *>(entry.second)) items.push_back(item);
        std::stable_sort(items.begin(), items.end(), [](Item *a, Item *b) {
            return a->getPriority() > b->getPriority();
        });
        std::unique_ptr<AssignmentStrategy> policy;
        if (document.data.assignmentStrategy == "RoleBased") policy.reset(new GuiRoleBased(workers, items, document.data.strategyOptions));
        else if (document.data.assignmentStrategy == "WorkloadBased") policy.reset(new GuiWorkloadBased(workers, items, document.data.strategyOptions));
        if (!policy) return 0;
        Item *selected = only.empty() ? nullptr : dynamic_cast<Item *>(component(only));
        std::size_t assigned = 0;
        for (Item *item : items) if (!item->getWorker() && item->getStatus() != "COMPLETED" && item->getStatus() != "CANCELLED") {
            if (selected && item != selected) continue;
            Worker *chosen = policy->assign(item, nullptr);
            if (chosen) { Assign assign(item, chosen); assign.execute(); ++assigned; }
        }
        refresh();
        return assigned;
    }
    void act(const OperationRequest &request) {
        const WorkflowSnapshot s = snapshot(0);
        const auto selected = std::find_if(s.components.begin(), s.components.end(),
            [&](const ComponentRecord &c) { return c.id == request.targetId; });
        require(selected != s.components.end() && selected->allowedActions.count(request.operation),
                "/operation", "This action is not available for the selected work and state.");
        if (request.operation == Operation::RenameComponent) {
            component(request.targetId)->setName(request.text); refresh(); return;
        }
        const std::vector<std::string> targets = leaves(request.targetId);
        require(!targets.empty(), "/componentId", "This group has no items.");
        std::size_t performed = 0;
        for (const std::string &id : targets) {
            Item *item = dynamic_cast<Item *>(component(id));
            if (selected->group) {
                const ComponentRecord &leaf = *std::find_if(s.components.begin(), s.components.end(),
                    [&](const ComponentRecord &c) { return c.id == id; });
                if (!leaf.allowedActions.count(request.operation)) continue;
            }
            std::unique_ptr<WorkflowCommand> command;
            switch (request.operation) {
            case Operation::SetPriority: item->setPriority(request.component.priority); break;
            case Operation::Assign: command.reset(new Assign(item, worker(request.relatedId))); break;
            case Operation::Unassign: item->assignWorker(nullptr); break;
            case Operation::Progress: command.reset(new Progress(item)); break;
            case Operation::Review:
                if (item->getStatus() != "IN_PROGRESS" && selected->group) continue;
                require(request.options.count("decision") &&
                        (option(request.options, "decision") == "approve" || option(request.options, "decision") == "reject"),
                        "/review", "Choose Approve or Reject in the review dialog.");
                require(option(request.options, "decision") != "reject" || !request.text.empty(), "/review/reason", "Enter a rejection reason.");
                if (item->getWorker() && item->getWorker()->getSupervisor())
                    item->getWorker()->getSupervisor()->setDecisionPolicy([request](Item *) { return option(request.options, "decision") == "approve"; });
                item->requestReview();
                if (option(request.options, "decision") == "reject") item->reject(request.text);
                break;
            case Operation::Reject:
                require(!request.text.empty(), "/reason", "Enter a rejection reason.");
                command.reset(new Reject(item, request.text)); break;
            case Operation::Escalate: command.reset(new Escalate(item)); break;
            case Operation::Cancel:
                if (item->getStatus() == "COMPLETED" || item->getStatus() == "CANCELLED") continue;
                command.reset(new Cancel(item)); break;
            default: throw InputError("/operation", "Unsupported item action.");
            }
            if (command) command->execute();
            ++performed;
        }
        require(performed != 0, "/state", "No items in this group can perform the selected action.");
        syncDependencies(); refresh();
    }
};

CoreWorkflowGateway::CoreWorkflowGateway() : revision(0)
{
    WorkflowDocument document; document.data.name = "Untitled workflow";
    state.reset(new Storage(document));
}
CoreWorkflowGateway::~CoreWorkflowGateway() {}
WorkflowSnapshot CoreWorkflowGateway::snapshot() { return state->snapshot(revision); }
WorkflowDocument CoreWorkflowGateway::exportDocument() { return state->document; }
void CoreWorkflowGateway::setInvalidationHandler(std::function<void()> callback) { invalidated = std::move(callback); }

OperationResult CoreWorkflowGateway::importDocument(const WorkflowDocument &document)
{
    try {
        std::unique_ptr<Storage> next(new Storage(document, true));
        require(revision != std::numeric_limits<std::uint64_t>::max(), "/revision", "The workflow revision is exhausted.");
        next->append("System", "Workflow imported.");
        state.swap(next); ++revision;
        if (invalidated) { try { invalidated(); } catch (...) {} }
        OperationResult result; result.ok = true; result.message = "Workflow imported."; return result;
    } catch (const DocumentError &e) {
        OperationResult result; result.message = "The workflow was not changed."; result.diagnostics = e.diagnostics(); return result;
    } catch (const InputError &e) { return failed(e.path, e.what()); }
    catch (const std::exception &e) { return failed("/", e.what()); }
    catch (const std::string &e) { return failed("/", e); }
    catch (const char *e) { return failed("/", e ? e : "The core rejected the import."); }
    catch (...) { return failed("/", "The workflow could not be imported."); }
}

OperationResult CoreWorkflowGateway::execute(const OperationRequest &request)
{
    try {
        WorkflowDocument document = state->document;
        const auto worker = [&](const std::string &id) -> WorkerRecord & {
            for (WorkerRecord &w : document.data.workers) if (w.id == id) return w;
            throw InputError("/workerId", "Unknown worker: " + id);
        };
        const auto component = [&](const std::string &id) -> ComponentRecord & {
            for (ComponentRecord &c : document.data.components) if (c.id == id) return c;
            throw InputError("/componentId", "Unknown component: " + id);
        };
        bool itemAction = false, automatic = false;
        switch (request.operation) {
        case Operation::CreateWorkflow:
            document = WorkflowDocument(); document.data.name = request.text; break;
        case Operation::AddWorker: document.data.workers.push_back(request.worker); break;
        case Operation::EditWorker: {
            require(request.worker.id.empty() || request.worker.id == request.targetId, "/worker/id", "A worker ID cannot be changed.");
            WorkerRecord &existing = worker(request.targetId);
            existing = request.worker; existing.id = request.targetId; break;
        }
        case Operation::SetSupervisor: worker(request.targetId).supervisorId = request.relatedId; break;
        case Operation::RemoveWorker: {
            worker(request.targetId);
            for (const WorkerRecord &w : document.data.workers)
                require(w.supervisorId != request.targetId, "/workerId", "Change this worker's subordinate relationships before removal.");
            for (const ComponentRecord &c : document.data.components)
                require(c.workerId != request.targetId, "/workerId", "Unassign this worker's items before removal.");
            auto &workers = document.data.workers;
            workers.erase(std::remove_if(workers.begin(), workers.end(), [&](const WorkerRecord &w) { return w.id == request.targetId; }), workers.end());
            break;
        }
        case Operation::AddItem:
        case Operation::AddGroup: {
            ComponentRecord c = request.component;
            c.group = request.operation == Operation::AddGroup;
            c.state = c.group ? "" : "AVAILABLE"; c.rejected = c.escalated = false;
            c.rejectionReason.clear(); c.allowedActions.clear();
            if (c.group) { c.workerId.clear(); c.deadline.clear(); }
            document.data.components.push_back(c); automatic = !c.group; break;
        }
        case Operation::SetStrategy:
            document.data.assignmentStrategy = request.text; document.data.strategyOptions = request.options; automatic = true; break;
        case Operation::AddDependency:
        case Operation::RemoveDependency: {
            ComponentRecord &dependent = component(request.targetId); component(request.relatedId);
            for (const std::string &id : state->leaves(dependent.id)) {
                const std::string status = component(id).state;
                require(status == "AVAILABLE" || status == "BLOCKED", "/dependencies", "Change dependencies before starting the dependent work.");
            }
            auto &links = document.data.dependencies;
            if (request.operation == Operation::AddDependency) {
                DependencyRecord edge; edge.prerequisiteId = request.relatedId; edge.dependentId = request.targetId; links.push_back(edge);
            } else {
                const auto it = std::find_if(links.begin(), links.end(), [&](const DependencyRecord &d) {
                    return d.prerequisiteId == request.relatedId && d.dependentId == request.targetId;
                });
                require(it != links.end(), "/dependencies", "The selected dependency no longer exists."); links.erase(it);
            }
            break;
        }
        case Operation::ConfigureNotifier:
        case Operation::ConfigureAudit: {
            const bool notifier = request.operation == Operation::ConfigureNotifier;
            validateObserver(request.options, notifier);
            if (request.targetId.empty()) {
                const std::string prefix = notifier ? "notifier." : "audit.";
                for (auto it = document.engineData.begin(); it != document.engineData.end(); )
                    if (it->first.compare(0, prefix.size(), prefix) == 0) it = document.engineData.erase(it); else ++it;
                for (const auto &entry : request.options) document.engineData[prefix + entry.first] = entry.second;
            } else {
                ComponentRecord &c = component(request.targetId);
                (notifier ? c.notifier : c.audit) = request.options;
            }
            break;
        }
        default:
            component(request.targetId);
            if (request.operation == Operation::Review)
                for (const std::string &id : state->leaves(request.targetId)) {
                    component(id).rejected = false; component(id).rejectionReason.clear();
                }
            itemAction = true; break;
        }
        std::unique_ptr<Storage> next(new Storage(document));
        if (itemAction) next->act(request);
        const std::size_t assigned = automatic ? next->assignUnassigned(
            request.operation == Operation::AddItem ? request.component.id : "") : 0;
        require(revision != std::numeric_limits<std::uint64_t>::max(), "/revision", "The workflow revision is exhausted.");
        OperationResult result; result.ok = true;
        result.selectedId = request.operation == Operation::AddItem || request.operation == Operation::AddGroup ? request.component.id : request.targetId;
        result.message = operationName(request.operation) + " completed.";
        if (request.operation == Operation::SetStrategy)
            result.message = request.text.empty() ? "Manual assignment selected."
                : "Assignment strategy applied: " + std::to_string(assigned) + " item(s) assigned.";
        if (request.operation == Operation::Progress || request.operation == Operation::Review) {
            const ComponentRecord &c = next->record(request.targetId);
            if (!c.group) result.message = c.name + ": " + (c.rejected ? "Review rejected." : c.state + ".");
        }
        next->append("System", result.message, result.selectedId);
        // Commit before invoking output adapters. A delivery failure must never
        // report an already-committed workflow action as rolled back.
        state.swap(next); ++revision;
        try {
            for (const ComponentRecord &c : state->document.data.components) {
                if (c.group) continue;
                const auto before = next->indexes.find(c.id);
                const ComponentRecord *old = before == next->indexes.end() ? nullptr : &next->document.data.components[before->second];
                if ((!old && !c.workerId.empty()) || (old && old->workerId != c.workerId))
                    state->publish("ASSIGNED", c.id, c.workerId.empty() ? c.name + " was unassigned." : c.name + " assigned to " + state->worker(c.workerId)->getName() + ".");
                if (old && c.state != old->state)
                    state->publish(c.state, c.id, c.name + ": " + old->state + " → " + c.state + ".");
                if (old && c.rejected && (!old->rejected || c.rejectionReason != old->rejectionReason))
                    state->publish("REJECTED", c.id, c.name + ": " + c.rejectionReason);
                if (old && c.escalated && !old->escalated)
                    state->publish("ESCALATED", c.id, c.name + " was escalated.");
            }
            state->publish("UPDATED", state->components.count(result.selectedId) ? result.selectedId : "", result.message);
        } catch (...) { result.message += " Some activity output could not be recorded."; }
        if (invalidated) { try { invalidated(); } catch (...) {} }
        return result;
    } catch (const DocumentError &e) {
        OperationResult result; result.message = "The workflow was not changed."; result.diagnostics = e.diagnostics(); return result;
    } catch (const InputError &e) { return failed(e.path, e.what()); }
    catch (const std::exception &e) { return failed("/operation", e.what()); }
    catch (const std::string &e) { return failed("/operation", e); }
    catch (const char *e) { return failed("/operation", e ? e : "The core rejected the action."); }
    catch (...) { return failed("/operation", "The operation could not be completed."); }
}
}

