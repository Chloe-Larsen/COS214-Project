#include "../../include/builder/WorkflowManager.h"
#include <iostream>
#include "../../include/command/Assign.h"
#include "../../include/command/Progress.h"

WorkflowManager::WorkflowManager() : builder(nullptr), invoker(new CommandInvoker())
{
}

WorkflowManager::~WorkflowManager()
{
	delete invoker;

	for (WorkflowInstance *instance : instances)
	{
		delete instance;
	}
	instances.clear();

	for (WorkflowDefinition *def : definitions)
	{
		delete def;
	}
	definitions.clear();
}

void WorkflowManager::setBuilder(WorkflowBuilder *builder)
{
	this->builder = builder;
}

WorkflowDefinition *WorkflowManager::constructWorkflow(WorkflowBlueprint &blueprint)
{
	if (!builder)
	{
		return nullptr;
	}

	builder->reset();
	builder->buildParticipantsAndRoles(blueprint);
	builder->buildStagesAndItems(blueprint);
	builder->buildDependencies(blueprint);
	builder->buildRoutingStrategy(blueprint);
	builder->buildApprovalChains(blueprint);
	builder->buildExternalAdapters(blueprint);

	WorkflowDefinition *def = builder->getResult();
	if (def)
	{
		definitions.push_back(def);
	}
	return def;
}

WorkflowInstance *WorkflowManager::startWorkflow(WorkflowDefinition *def)
{
	WorkflowInstance *instance = new WorkflowInstance(*def->createInstance());
	instances.push_back(instance);
	instance->progress();
	return instance;
}

void WorkflowManager::assignWork(Item *item)
{
	if (!item || !invoker)
		return;
	WorkflowCommand *assignCmd = new Assign(item);
	invoker->executeCommand(assignCmd);
}

void WorkflowManager::completeWork(Item *item)
{
	if (!item || !invoker)
		return;
	WorkflowCommand *completeCmd = new Progress(item);
	invoker->executeCommand(completeCmd);
}

std::vector<WorkflowCommand *> WorkflowManager::getCommandHistory()
{
	if (invoker)
	{
		return invoker->getHistory();
	}
	return std::vector<WorkflowCommand *>();
}

void WorkflowManager::notifyOutcome(WorkflowInstance *instance)
{
	if (!instance)
		return;
	std::cout << "[WorkflowManager] Outcome recorded for workflow instance: "
			  << instance->getId() << " Status: "
			  << instance->getStatus() << std::endl;
}
