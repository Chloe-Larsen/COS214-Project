#include "CoreWorkflowGateway.h"

namespace tividy {

std::shared_ptr<IWorkflowGateway> createGateway()
{
    return std::shared_ptr<IWorkflowGateway>(new CoreWorkflowGateway());
}

} // namespace tividy
