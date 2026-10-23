#include "../../include/builder/WorkflowManager.h"

WorkflowManager::WorkflowManager()
{
	// TODO - implement WorkflowManager::WorkflowManager
	throw "Not yet implemented";
}

void WorkflowManager::setBuilder(WorkflowBuilder *builder)
{
	this->builder = builder;
}

WorkflowDefinition *WorkflowManager::constructWorkflow(WorkflowBlueprint &blueprint)
{
	// TODO - implement WorkflowManager::constructWorkflow
	throw "Not yet implemented";
}

WorkflowInstance *WorkflowManager::startWorkflow(WorkflowDefinition *def)
{
	// TODO - implement WorkflowManager::startWorkflow
	throw "Not yet implemented";
}

void WorkflowManager::assignWork(Item *item)
{
	// TODO - implement WorkflowManager::assignWork
	throw "Not yet implemented";
}

void WorkflowManager::completeWork(Item *item)
{
	// TODO - implement WorkflowManager::completeWork
	throw "Not yet implemented";
}

std::vector<WorkflowCommand *> WorkflowManager::getCommandHistory()
{
	// TODO - implement WorkflowManager::getCommandHistory
	throw "Not yet implemented";
}

void WorkflowManager::notifyOutcome(WorkflowInstance *instance)
{
	// TODO - implement WorkflowManager::notifyOutcome
	throw "Not yet implemented";
}
