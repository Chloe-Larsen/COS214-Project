#include "JsonCodec.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <climits>
#include <sstream>
#include <utility>

namespace tividy {
using Json = nlohmann::json;
namespace {
std::string describe(const std::vector<Diagnostic> &errors)
{
    std::ostringstream out;
    for (std::size_t i = 0; i < errors.size(); ++i) {
        if (i) out << '\n';
        if (errors[i].line > 0) out << "Line " << errors[i].line << ", column " << errors[i].column << ": ";
        else out << errors[i].location << ": ";
        out << errors[i].message;
    }
    return out.str();
}
struct Reader {
    std::vector<Diagnostic> errors;
    void keys(const Json &object, const std::set<std::string> &allowed, const std::string &path) {
        for (auto field = object.begin(); field != object.end(); ++field)
            if (!allowed.count(field.key())) fail(path + "/" + field.key(), "Unrecognised field. Put engine-specific saved data in engineData.");
    }
    void fail(const std::string &path, const std::string &message) {
        if (errors.size() < 100) { Diagnostic d; d.location = path; d.message = message; errors.push_back(d); }
    }
    std::string text(const Json &object, const std::string &key, const std::string &path, bool required = false) {
        if (!object.is_object()) { fail(path, "Expected an object."); return {}; }
        const auto it = object.find(key);
        if (it == object.end()) { if (required) fail(path + "/" + key, "Required field is missing."); return {}; }
        if (!it->is_string()) { fail(path + "/" + key, "Expected a string."); return {}; }
        const std::string value = it->get<std::string>();
        if (value.size() > 65536) fail(path + "/" + key, "String exceeds 64 KiB.");
        if (required && value.empty()) fail(path + "/" + key, "Value must not be empty.");
        return value;
    }
    int integer(const Json &object, const std::string &key, const std::string &path) {
        const auto it = object.find(key);
        if (it == object.end()) return 0;
        if (!it->is_number_integer() || (it->is_number_unsigned() && it->get<unsigned long long>() > INT_MAX)
            || (!it->is_number_unsigned() && (it->get<long long>() < INT_MIN || it->get<long long>() > INT_MAX))) {
            fail(path + "/" + key, "Expected a 32-bit integer."); return 0;
        }
        return it->get<int>();
    }
    bool boolean(const Json &object, const std::string &key, const std::string &path) {
        const auto it = object.find(key);
        if (it == object.end()) return false;
        if (!it->is_boolean()) { fail(path + "/" + key, "Expected true or false."); return false; }
        return it->get<bool>();
    }
    Json array(const Json &object, const std::string &key, const std::string &path, std::size_t limit) {
        const auto it = object.find(key);
        if (it == object.end() || !it->is_array()) { fail(path + "/" + key, "Expected an array."); return Json::array(); }
        if (it->size() > limit) { fail(path + "/" + key, "Too many records."); return Json::array(); }
        return *it;
    }
    std::map<std::string, std::string> options(const Json &object, const std::string &key, const std::string &path) {
        std::map<std::string, std::string> result;
        const auto it = object.find(key);
        if (it == object.end()) return result;
        if (!it->is_object() || it->size() > 1000) { fail(path + "/" + key, "Expected an object with at most 1,000 string values."); return result; }
        for (auto field = it->begin(); field != it->end(); ++field) result[field.key()] = text(*it, field.key(), path + "/" + key);
        return result;
    }
};
}

DocumentError::DocumentError(std::vector<Diagnostic> values) : std::runtime_error(describe(values)), errors(std::move(values)) {}
const std::vector<Diagnostic> &DocumentError::diagnostics() const { return errors; }

WorkflowDocument JsonCodec::decode(const std::string &utf8)
{
    Reader r;
    if (utf8.size() > 64 * 1024 * 1024) { r.fail("/", "File exceeds the 64 MiB import limit."); throw DocumentError(r.errors); }
    Json root;
    try {
        root = Json::parse(utf8, [&r](int depth, Json::parse_event_t, Json &) {
            if (depth > 64) { r.fail("/", "JSON nesting exceeds 64 levels."); throw DocumentError(r.errors); }
            return true;
        });
    }
    catch (const Json::parse_error &error) {
        Diagnostic d; d.location = "/"; d.message = error.what(); d.line = 1; d.column = 1;
        const std::size_t end = std::min(utf8.size(), error.byte > 0 ? error.byte - 1 : 0);
        for (std::size_t i = 0; i < end; ++i) {
            const unsigned char ch = static_cast<unsigned char>(utf8[i]);
            if (ch == '\n') { ++d.line; d.column = 1; }
            else if ((ch & 0xc0) != 0x80) ++d.column;
        }
        r.errors.push_back(d); throw DocumentError(r.errors);
    }
    if (!root.is_object()) { r.fail("/", "Expected a workflow object."); throw DocumentError(r.errors); }
    r.keys(root, {"schemaVersion", "name", "assignmentStrategy", "strategyOptions", "engineData", "workers", "components", "dependencies", "activity"}, "");
    if (r.integer(root, "schemaVersion", "") != 1) r.fail("/schemaVersion", "Expected schemaVersion 1.");
    WorkflowDocument document;
    WorkflowSnapshot &s = document.data;
    s.name = r.text(root, "name", "", true);
    s.assignmentStrategy = r.text(root, "assignmentStrategy", "");
    s.strategyOptions = r.options(root, "strategyOptions", "");
    document.engineData = r.options(root, "engineData", "");
    const Json workers = r.array(root, "workers", "", 10000);
    std::set<std::string> workerIds, componentIds;
    for (std::size_t i = 0; i < workers.size(); ++i) {
        const std::string path = "/workers/" + std::to_string(i);
        WorkerRecord w;
        if (!workers[i].is_object()) { r.fail(path, "Expected a worker object."); continue; }
        r.keys(workers[i], {"id", "name", "role", "email", "supervisorId"}, path);
        w.id = r.text(workers[i], "id", path, true); w.name = r.text(workers[i], "name", path, true);
        w.role = r.text(workers[i], "role", path, true); w.email = r.text(workers[i], "email", path);
        w.supervisorId = r.text(workers[i], "supervisorId", path);
        if (!workerIds.insert(w.id).second) r.fail(path + "/id", "Duplicate worker ID.");
        s.workers.push_back(w);
    }
    const Json components = r.array(root, "components", "", 10000);
    for (std::size_t i = 0; i < components.size(); ++i) {
        const std::string path = "/components/" + std::to_string(i);
        const Json &j = components[i];
        if (!j.is_object()) { r.fail(path, "Expected a component object."); continue; }
        r.keys(j, {"id", "name", "parentId", "kind", "workerId", "state", "deadline", "priority", "rejected", "escalated", "rejectionReason", "notifier", "audit"}, path);
        ComponentRecord c;
        c.id = r.text(j, "id", path, true); c.name = r.text(j, "name", path, true);
        c.parentId = r.text(j, "parentId", path); c.workerId = r.text(j, "workerId", path);
        c.state = r.text(j, "state", path); c.deadline = r.text(j, "deadline", path);
        c.priority = r.integer(j, "priority", path);
        const std::string kind = r.text(j, "kind", path, true);
        if (kind != "item" && kind != "group") r.fail(path + "/kind", "Expected item or group.");
        c.group = kind == "group";
        c.rejected = r.boolean(j, "rejected", path); c.escalated = r.boolean(j, "escalated", path);
        c.rejectionReason = r.text(j, "rejectionReason", path);
        c.notifier = r.options(j, "notifier", path); c.audit = r.options(j, "audit", path);
        if (!componentIds.insert(c.id).second) r.fail(path + "/id", "Duplicate component ID.");
        s.components.push_back(c);
    }
    for (std::size_t i = 0; i < s.components.size(); ++i) {
        const ComponentRecord &c = s.components[i]; const std::string path = "/components/" + std::to_string(i);
        if (!c.parentId.empty() && !componentIds.count(c.parentId)) r.fail(path + "/parentId", "Unknown component ID.");
        if (!c.workerId.empty() && !workerIds.count(c.workerId)) r.fail(path + "/workerId", "Unknown worker ID.");
    }
    for (std::size_t i = 0; i < s.workers.size(); ++i)
        if (!s.workers[i].supervisorId.empty() && !workerIds.count(s.workers[i].supervisorId))
            r.fail("/workers/" + std::to_string(i) + "/supervisorId", "Unknown worker ID.");
    const Json edges = r.array(root, "dependencies", "", 50000);
    std::set<std::pair<std::string, std::string> > pairs;
    for (std::size_t i = 0; i < edges.size(); ++i) {
        const std::string path = "/dependencies/" + std::to_string(i);
        if (!edges[i].is_object()) { r.fail(path, "Expected a dependency object."); continue; }
        r.keys(edges[i], {"prerequisiteId", "dependentId"}, path);
        DependencyRecord edge;
        edge.prerequisiteId = r.text(edges[i], "prerequisiteId", path, true);
        edge.dependentId = r.text(edges[i], "dependentId", path, true);
        if (!componentIds.count(edge.prerequisiteId)) r.fail(path + "/prerequisiteId", "Unknown component ID.");
        if (!componentIds.count(edge.dependentId)) r.fail(path + "/dependentId", "Unknown component ID.");
        if (!pairs.insert(std::make_pair(edge.prerequisiteId, edge.dependentId)).second) r.fail(path, "Duplicate dependency record.");
        s.dependencies.push_back(edge);
    }
    const Json history = root.count("activity") ? r.array(root, "activity", "", 100000) : Json::array();
    for (std::size_t i = 0; i < history.size(); ++i) {
        const std::string path = "/activity/" + std::to_string(i);
        if (!history[i].is_object()) { r.fail(path, "Expected an activity object."); continue; }
        r.keys(history[i], {"id", "timestamp", "category", "componentId", "componentName", "message"}, path);
        ActivityRecord a;
        a.id = r.text(history[i], "id", path, true); a.timestamp = r.text(history[i], "timestamp", path, true);
        a.category = r.text(history[i], "category", path, true); a.componentId = r.text(history[i], "componentId", path);
        a.componentName = r.text(history[i], "componentName", path); a.message = r.text(history[i], "message", path, true);
        s.activity.push_back(a);
    }
    if (!r.errors.empty()) throw DocumentError(r.errors);
    // Parent/dependency/supervisor cycles and lifecycle policy remain core rules.
    return document;
}

std::string JsonCodec::encode(const WorkflowDocument &document)
{
    const WorkflowSnapshot &s = document.data;
    Json root = {{"schemaVersion", 1}, {"name", s.name}, {"assignmentStrategy", s.assignmentStrategy},
                 {"strategyOptions", s.strategyOptions}, {"engineData", document.engineData}};
    root["workers"] = Json::array(); root["components"] = Json::array();
    root["dependencies"] = Json::array(); root["activity"] = Json::array();
    for (const WorkerRecord &w : s.workers)
        root["workers"].push_back({{"id",w.id},{"name",w.name},{"role",w.role},{"email",w.email},{"supervisorId",w.supervisorId}});
    for (const ComponentRecord &c : s.components)
        root["components"].push_back({{"id",c.id},{"parentId",c.parentId},{"name",c.name},{"kind",c.group ? "group" : "item"},
            {"state",c.state},{"workerId",c.workerId},{"priority",c.priority},{"deadline",c.deadline},
            {"rejected",c.rejected},{"escalated",c.escalated},{"rejectionReason",c.rejectionReason},{"notifier",c.notifier},{"audit",c.audit}});
    for (const DependencyRecord &d : s.dependencies)
        root["dependencies"].push_back({{"prerequisiteId",d.prerequisiteId},{"dependentId",d.dependentId}});
    for (const ActivityRecord &a : s.activity)
        root["activity"].push_back({{"id",a.id},{"timestamp",a.timestamp},{"category",a.category},{"componentId",a.componentId},
            {"componentName",a.componentName},{"message",a.message}});
    const std::string result = root.dump(2) + "\n";
    decode(result); // An exported document must satisfy the same transport rules.
    return result;
}
}
