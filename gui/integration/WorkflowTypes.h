#ifndef TIVIDY_GUI_WORKFLOWTYPES_H
#define TIVIDY_GUI_WORKFLOWTYPES_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace tividy {

/** User requests understood by the GUI/engine integration contract. */
enum class Operation {
    CreateWorkflow, AddItem, AddGroup, RenameComponent, SetPriority,
    Assign, Unassign, Progress, Review, Reject, Escalate, Cancel,
    AddWorker, EditWorker, RemoveWorker, SetSupervisor, SetStrategy,
    AddDependency, RemoveDependency, ConfigureNotifier, ConfigureAudit
};

/** Stable identity, editable fields and supervisor supplied by the engine. */
struct WorkerRecord {
    std::string id, name, role, email, supervisorId;
    int workload = 0;
};

/** Immutable presentation record; allowedActions is authoritative engine policy. */
struct ComponentRecord {
    std::string id, parentId, name, state, workerId, deadline, rejectionReason;
    bool group = false;
    bool rejected = false;
    bool escalated = false;
    int priority = 0;
    std::set<Operation> allowedActions;
    std::map<std::string, std::string> notifier, audit;
};

/** A prerequisite -> dependent edge; endpoints may be items or groups. */
struct DependencyRecord {
    std::string prerequisiteId, dependentId;
};

/** One actual engine event or output record, with stable de-duplication identity. */
struct ActivityRecord {
    std::string id, timestamp, category, componentId, componentName, message;
};

/** A complete copied view of one workflow. Never contains engine pointers. */
struct WorkflowSnapshot {
    std::string name;
    std::uint64_t revision = 0;
    bool dirty = false;
    std::vector<ComponentRecord> components;
    std::vector<WorkerRecord> workers;
    std::vector<DependencyRecord> dependencies;
    std::vector<ActivityRecord> activity;
    std::string assignmentStrategy;
    std::map<std::string, std::string> strategyOptions;
    std::set<Operation> capabilities;
    /** Editable fields advertised by the connection, independent of domain policy. */
    std::set<std::string> workerInputFields = {"name", "role", "email", "supervisor"};
    bool canImport = false;
    bool canExport = false;
    std::string availabilityMessage;
};

/** Validated transport data. Actions/capabilities are intentionally never imported. */
struct WorkflowDocument {
    WorkflowSnapshot data;
    std::map<std::string, std::string> engineData;
};

/** Context and data for a single operation. See docs/CORE_INTEGRATION.md. */
struct OperationRequest {
    Operation operation = Operation::CreateWorkflow;
    std::string targetId, relatedId, text;
    ComponentRecord component;
    WorkerRecord worker;
    std::map<std::string, std::string> options;
};

/** Structured error; location is a JSON path or an engine field identifier. */
struct Diagnostic {
    std::string location, message;
    int line = 0, column = 0;
};

/** Outcome of an engine transaction. Failure must leave the engine unchanged. */
struct OperationResult {
    bool ok = false;
    std::string message;
    std::vector<Diagnostic> diagnostics;
    std::string selectedId;
};

/** Human-readable caption used by menus and diagnostics. */
std::string operationName(Operation operation);

} // namespace tividy
#endif
