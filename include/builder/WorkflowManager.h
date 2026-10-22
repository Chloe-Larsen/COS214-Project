#ifndef WORKFLOWMANAGER_H
#define WORKFLOWMANAGER_H

#include <vector>

#include "WorkflowBuilder.h"
#include "../command/CommandInvoker.h"
#include "WorkflowDefinition.h"
#include "../WorkflowInstance.h"
class WorkflowBlueprint;
#include "../composite/Item.h"
#include "../command/WorkflowCommand.h"

class WorkflowManager
{
private:
	WorkflowBuilder *builder;
	CommandInvoker *invoker;
	std::vector<WorkflowDefinition *> definitions;
	std::vector<WorkflowInstance *> instances;

public:
	WorkflowManager();
	~WorkflowManager();
	void setBuilder(WorkflowBuilder *builder);
	WorkflowDefinition *constructWorkflow(WorkflowBlueprint &blueprint);
	WorkflowInstance *startWorkflow(WorkflowDefinition *def);
	void assignWork(Item *item);
	void completeWork(Item *item);
	std::vector<WorkflowCommand *> getCommandHistory();
	void notifyOutcome(WorkflowInstance *instance);
};

#endif
