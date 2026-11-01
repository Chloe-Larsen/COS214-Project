#include "JsonCodec.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <algorithm>
#include <climits>
#include <cmath>
#include <sstream>
#include <utility>

namespace tividy {
namespace {
QString q(const std::string &s) { return QString::fromStdString(s); }
std::string describe(const std::vector<Diagnostic> &errors)
{
    std::ostringstream out;
    for (std::size_t i = 0; i < errors.size(); ++i) {
        if (i) out << '\n';
        const Diagnostic &d = errors[i];
        if (d.line > 0) out << "Line " << d.line << ", column " << d.column << ": ";
        else out << d.location << ": ";
        out << d.message;
    }
    return out.str();
}
Diagnostic syntaxError(const std::string &input, std::size_t offset, const std::string &message)
{
    Diagnostic d; d.location = "/"; d.message = message; d.line = d.column = 1;
    for (std::size_t i = 0; i < std::min(offset, input.size()); ++i) {
        const unsigned char ch = static_cast<unsigned char>(input[i]);
        if (ch == '\n') { ++d.line; d.column = 1; }
        else if ((ch & 0xc0) != 0x80) ++d.column;
    }
    return d;
}
struct Reader {
    std::vector<Diagnostic> errors;
    void fail(const std::string &path, const std::string &message) {
        if (errors.size() < 100) { Diagnostic d; d.location = path; d.message = message; errors.push_back(d); }
    }
    void keys(const QJsonObject &object, const std::set<std::string> &allowed, const std::string &path) {
        for (auto it = object.begin(); it != object.end(); ++it)
            if (!allowed.count(it.key().toStdString())) fail(path + "/" + it.key().toStdString(), "Unrecognised field.");
    }
    std::string text(const QJsonObject &object, const std::string &key, const std::string &path, bool required = false) {
        const QJsonValue value = object.value(q(key));
        if (value.isUndefined()) { if (required) fail(path + "/" + key, "Required field is missing."); return {}; }
        if (!value.isString()) { fail(path + "/" + key, "Expected a string."); return {}; }
        const std::string result = value.toString().toStdString();
        if (result.size() > 65536) fail(path + "/" + key, "String exceeds 64 KiB.");
        if (required && value.toString().trimmed().isEmpty()) fail(path + "/" + key, "Value must not be empty.");
        return result;
    }
    int integer(const QJsonObject &object, const std::string &key, const std::string &path) {
        const QJsonValue value = object.value(q(key));
        if (value.isUndefined()) return 0;
        const double n = value.toDouble();
        if (!value.isDouble() || !std::isfinite(n) || std::floor(n) != n || n < INT_MIN || n > INT_MAX) {
            fail(path + "/" + key, "Expected a 32-bit integer."); return 0;
        }
        return static_cast<int>(n);
    }
    bool boolean(const QJsonObject &object, const std::string &key, const std::string &path) {
        const QJsonValue value = object.value(q(key));
        if (value.isUndefined()) return false;
        if (!value.isBool()) { fail(path + "/" + key, "Expected true or false."); return false; }
        return value.toBool();
    }
    QJsonArray array(const QJsonObject &object, const std::string &key, const std::string &path, std::size_t limit) {
        const QJsonValue value = object.value(q(key));
        if (!value.isArray()) { fail(path + "/" + key, "Expected an array."); return {}; }
        if (static_cast<std::size_t>(value.toArray().size()) > limit) { fail(path + "/" + key, "Too many records."); return {}; }
        return value.toArray();
    }
    std::map<std::string, std::string> options(const QJsonObject &object, const std::string &key, const std::string &path) {
        std::map<std::string, std::string> result;
        const QJsonValue value = object.value(q(key));
        if (value.isUndefined()) return result;
        if (!value.isObject() || value.toObject().size() > 1000) {
            fail(path + "/" + key, "Expected an object with at most 1,000 string values."); return result;
        }
        const QJsonObject fields = value.toObject();
        for (auto it = fields.begin(); it != fields.end(); ++it) {
            const std::string name = it.key().toStdString();
            if (name.size() > 65536) fail(path + "/" + key, "Option name exceeds 64 KiB.");
            result[name] = text(fields, name, path + "/" + key);
        }
        return result;
    }
};
QJsonObject options(const std::map<std::string, std::string> &values)
{
    QJsonObject result;
    for (const auto &entry : values) result.insert(q(entry.first), q(entry.second));
    return result;
}
}
DocumentError::DocumentError(std::vector<Diagnostic> values)
    : std::runtime_error(describe(values)), errors(std::move(values)) {}
const std::vector<Diagnostic> &DocumentError::diagnostics() const { return errors; }

WorkflowDocument JsonCodec::decode(const std::string &utf8)
{
    Reader r;
    if (utf8.size() > 64 * 1024 * 1024) {
        r.fail("/", "File exceeds the 64 MiB import limit."); throw DocumentError(r.errors);
    }
    // Braces in strings and escaped quotes do not contribute to nesting.
    int depth = 0; bool quoted = false, escaped = false;
    for (std::size_t i = 0; i < utf8.size(); ++i) {
        const char ch = utf8[i];
        if (quoted) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') quoted = false;
        } else if (ch == '"') quoted = true;
        else if (ch == '{' || ch == '[') {
            if (++depth > 64) throw DocumentError({syntaxError(utf8, i, "JSON nesting exceeds 64 levels.")});
        } else if (ch == '}' || ch == ']') --depth;
    }
    QJsonParseError error;
    const QJsonDocument parsed = QJsonDocument::fromJson(QByteArray(utf8.data(), static_cast<int>(utf8.size())), &error);
    if (error.error != QJsonParseError::NoError)
        throw DocumentError({syntaxError(utf8, static_cast<std::size_t>(std::max(0, error.offset)), error.errorString().toStdString())});
    if (!parsed.isObject()) { r.fail("/", "Expected a workflow object."); throw DocumentError(r.errors); }
    const QJsonObject root = parsed.object();
    r.keys(root, {"schemaVersion", "name", "assignmentStrategy", "strategyOptions", "engineData", "workers", "components", "dependencies", "activity"}, "");
    if (r.integer(root, "schemaVersion", "") != 1) r.fail("/schemaVersion", "Expected schemaVersion 1.");
    WorkflowDocument document;
    WorkflowSnapshot &s = document.data;
    s.name = r.text(root, "name", "", true);
    s.assignmentStrategy = r.text(root, "assignmentStrategy", "");
    s.strategyOptions = r.options(root, "strategyOptions", "");
    document.engineData = r.options(root, "engineData", "");
    const QJsonArray workers = r.array(root, "workers", "", 10000);
    std::set<std::string> workerIds, componentIds;
    for (std::size_t i = 0; i < static_cast<std::size_t>(workers.size()); ++i) {
        const std::string path = "/workers/" + std::to_string(i);
        WorkerRecord w;
        if (!workers[static_cast<int>(i)].isObject()) { r.fail(path, "Expected a worker object."); continue; }
        r.keys(workers[static_cast<int>(i)].toObject(), {"id", "name", "role", "email", "supervisorId"}, path);
        w.id = r.text(workers[static_cast<int>(i)].toObject(), "id", path, true); w.name = r.text(workers[static_cast<int>(i)].toObject(), "name", path, true);
        w.role = r.text(workers[static_cast<int>(i)].toObject(), "role", path, true); w.email = r.text(workers[static_cast<int>(i)].toObject(), "email", path);
        w.supervisorId = r.text(workers[static_cast<int>(i)].toObject(), "supervisorId", path);
        if (!workerIds.insert(w.id).second) r.fail(path + "/id", "Duplicate worker ID.");
        s.workers.push_back(w);
    }
    const QJsonArray components = r.array(root, "components", "", 10000);
    for (std::size_t i = 0; i < static_cast<std::size_t>(components.size()); ++i) {
        const std::string path = "/components/" + std::to_string(i);
        if (!components[static_cast<int>(i)].isObject()) { r.fail(path, "Expected a component object."); continue; }
        const QJsonObject j = components[static_cast<int>(i)].toObject();
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
        const ComponentRecord &c = s.components[static_cast<int>(i)]; const std::string path = "/components/" + std::to_string(i);
        if (!c.parentId.empty() && !componentIds.count(c.parentId)) r.fail(path + "/parentId", "Unknown component ID.");
        if (!c.workerId.empty() && !workerIds.count(c.workerId)) r.fail(path + "/workerId", "Unknown worker ID.");
    }
    for (std::size_t i = 0; i < s.workers.size(); ++i)
        if (!s.workers[static_cast<int>(i)].supervisorId.empty() && !workerIds.count(s.workers[static_cast<int>(i)].supervisorId))
            r.fail("/workers/" + std::to_string(i) + "/supervisorId", "Unknown worker ID.");
    const QJsonArray edges = r.array(root, "dependencies", "", 50000);
    std::set<std::pair<std::string, std::string> > pairs;
    for (std::size_t i = 0; i < static_cast<std::size_t>(edges.size()); ++i) {
        const std::string path = "/dependencies/" + std::to_string(i);
        if (!edges[static_cast<int>(i)].isObject()) { r.fail(path, "Expected a dependency object."); continue; }
        r.keys(edges[static_cast<int>(i)].toObject(), {"prerequisiteId", "dependentId"}, path);
        DependencyRecord edge;
        edge.prerequisiteId = r.text(edges[static_cast<int>(i)].toObject(), "prerequisiteId", path, true);
        edge.dependentId = r.text(edges[static_cast<int>(i)].toObject(), "dependentId", path, true);
        if (!componentIds.count(edge.prerequisiteId)) r.fail(path + "/prerequisiteId", "Unknown component ID.");
        if (!componentIds.count(edge.dependentId)) r.fail(path + "/dependentId", "Unknown component ID.");
        if (!pairs.insert(std::make_pair(edge.prerequisiteId, edge.dependentId)).second) r.fail(path, "Duplicate dependency record.");
        s.dependencies.push_back(edge);
    }
    const QJsonArray history = root.contains("activity") ? r.array(root, "activity", "", 100000) : QJsonArray();
    for (std::size_t i = 0; i < static_cast<std::size_t>(history.size()); ++i) {
        const std::string path = "/activity/" + std::to_string(i);
        if (!history[static_cast<int>(i)].isObject()) { r.fail(path, "Expected an activity object."); continue; }
        r.keys(history[static_cast<int>(i)].toObject(), {"id", "timestamp", "category", "componentId", "componentName", "message"}, path);
        ActivityRecord a;
        a.id = r.text(history[static_cast<int>(i)].toObject(), "id", path, true); a.timestamp = r.text(history[static_cast<int>(i)].toObject(), "timestamp", path, true);
        a.category = r.text(history[static_cast<int>(i)].toObject(), "category", path, true); a.componentId = r.text(history[static_cast<int>(i)].toObject(), "componentId", path);
        a.componentName = r.text(history[static_cast<int>(i)].toObject(), "componentName", path); a.message = r.text(history[static_cast<int>(i)].toObject(), "message", path, true);
        s.activity.push_back(a);
    }
    if (!r.errors.empty()) throw DocumentError(r.errors);
    // The gateway validates hierarchy/dependency cycles and lifecycle policy.
    return document;
}

std::string JsonCodec::encode(const WorkflowDocument &document)
{
    const WorkflowSnapshot &s = document.data;
    QJsonObject root;
    root["schemaVersion"] = 1; root["name"] = q(s.name);
    root["assignmentStrategy"] = q(s.assignmentStrategy);
    root["strategyOptions"] = options(s.strategyOptions); root["engineData"] = options(document.engineData);
    QJsonArray workers, components, dependencies, activity;
    for (const WorkerRecord &w : s.workers) {
        QJsonObject j; j["id"] = q(w.id); j["name"] = q(w.name); j["role"] = q(w.role);
        j["email"] = q(w.email); j["supervisorId"] = q(w.supervisorId); workers.append(j);
    }
    for (const ComponentRecord &c : s.components) {
        QJsonObject j; j["id"] = q(c.id); j["name"] = q(c.name); j["parentId"] = q(c.parentId);
        j["kind"] = c.group ? "group" : "item"; j["state"] = q(c.state); j["workerId"] = q(c.workerId);
        j["priority"] = c.priority; j["deadline"] = q(c.deadline); j["rejected"] = c.rejected;
        j["escalated"] = c.escalated; j["rejectionReason"] = q(c.rejectionReason);
        j["notifier"] = options(c.notifier); j["audit"] = options(c.audit); components.append(j);
    }
    for (const DependencyRecord &d : s.dependencies) {
        QJsonObject j; j["prerequisiteId"] = q(d.prerequisiteId); j["dependentId"] = q(d.dependentId); dependencies.append(j);
    }
    for (const ActivityRecord &a : s.activity) {
        QJsonObject j; j["id"] = q(a.id); j["timestamp"] = q(a.timestamp); j["category"] = q(a.category);
        j["componentId"] = q(a.componentId); j["componentName"] = q(a.componentName); j["message"] = q(a.message); activity.append(j);
    }
    root["workers"] = workers; root["components"] = components;
    root["dependencies"] = dependencies; root["activity"] = activity;
    const std::string result = QJsonDocument(root).toJson(QJsonDocument::Indented).toStdString();
    decode(result);
    return result;
}
}
