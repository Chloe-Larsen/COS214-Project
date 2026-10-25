#include "../../include/builder/WorkflowDefinition.h"
#include "../../include/WorkflowInstance.h"
#include "../../include/composite/WorkComponent.h"

WorkflowInstance WorkflowDefinition::createInstance()
{
	WorkflowInstance *inst = new WorkflowInstance();
	inst->buildFromDefinition(this);

	return inst;
}

WorkComponent *WorkflowDefinition::getRootTemplate()
{
	return this->rootTemplate;
}
