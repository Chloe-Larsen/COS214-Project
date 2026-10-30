#include "CoreWorkflowGateway.h"
#include "WorkflowManager.h"
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

static OperationResult unavailable()
{
    OperationResult result;
    result.message = "This operation is not available in the connected engine.";
    return result;
}

CoreWorkflowGateway::CoreWorkflowGateway(std::shared_ptr<WorkflowManager> value, CoreBindings functions)
    : manager(std::move(value)), bindings(std::move(functions)), revision(0)
{
    if (!manager) throw std::invalid_argument("A workflow manager is required.");
}

CoreWorkflowGateway::~CoreWorkflowGateway()
{
    // WorkflowController disconnects on its engine queue before draining it.
    // Do not invoke engine functions again on the GUI destruction thread.
}

WorkflowSnapshot CoreWorkflowGateway::snapshot()
{
    if (bindings.read) {
        WorkflowSnapshot result = bindings.read();
        result.canImport = result.canImport && static_cast<bool>(bindings.import);
        result.canExport = result.canExport && static_cast<bool>(bindings.exportState);
        if (!bindings.execute) result.capabilities.clear();
        return result;
    }
    WorkflowSnapshot result;
    result.revision = revision;
    result.dirty = revision > 0;
    result.capabilities.insert(Operation::AddWorker);
    result.workerInputFields = {"name", "role"};
    result.availabilityMessage = "Worker creation is connected. Workflow actions need the updated engine integration.";
    const std::vector<Worker *> actual = manager->getWorkers();
    for (Worker *person : actual) {
        if (!person) continue;
        WorkerRecord value;
        value.id = person->getId();
        value.name = person->getName();
        value.role = person->getRole();
        value.email = person->getEmail();
        if (person->getSupervisor()) value.supervisorId = person->getSupervisor()->getId();
        // getWorkload() uses the ZIP's unsafe iterator; do not call it here.
        result.workers.push_back(value);
    }
    return result;
}

OperationResult CoreWorkflowGateway::execute(const OperationRequest &request)
{
    if (bindings.execute) return bindings.execute(request);
    if (bindings.read || request.operation != Operation::AddWorker) return unavailable();
    const WorkerRecord &input = request.worker;
    OperationResult result;
    if (input.id.empty() || input.name.empty() || input.role.empty()) {
        result.message = "Worker ID, name and role are required.";
        return result;
    }
    for (Worker *existing : manager->getWorkers()) {
        if (existing && existing->getId() == input.id) {
            result.message = "A worker with this ID already exists.";
            return result;
        }
    }
    if (!input.email.empty() || !input.supervisorId.empty()) {
        result.message = "This engine cannot yet accept email or superior settings. Leave those fields empty.";
        return result;
    }
    std::unique_ptr<Worker> person(new Worker(input.id, input.name, input.role));
    manager->addWorker(person.get());
    person.release(); // addWorker transfers ownership to WorkflowManager.
    ++revision;
    result.ok = true;
    result.message = "Worker added.";
    if (invalidated) invalidated();
    return result;
}

OperationResult CoreWorkflowGateway::importDocument(const WorkflowDocument &document)
{
    return bindings.import ? bindings.import(document) : unavailable();
}

WorkflowDocument CoreWorkflowGateway::exportDocument()
{
    if (!bindings.exportState) throw std::runtime_error("Saving is not available in the connected engine.");
    return bindings.exportState();
}

void CoreWorkflowGateway::setInvalidationHandler(std::function<void()> handler)
{
    invalidated = std::move(handler);
    if (bindings.subscribe) bindings.subscribe(invalidated);
}

} // namespace tividy
