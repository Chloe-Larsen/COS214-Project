#include "CoreWorkflowGateway.h"
#include "WorkflowManager.h"

namespace tividy {

std::shared_ptr<IWorkflowGateway> createGateway()
{
    // This is the only composition root a teammate changes to install completed
    // engine callbacks. An empty binding set accurately exposes the supplied ZIP.
    std::shared_ptr<WorkflowManager> manager(new WorkflowManager());
    return std::shared_ptr<IWorkflowGateway>(new CoreWorkflowGateway(manager, CoreBindings()));
}

} // namespace tividy
