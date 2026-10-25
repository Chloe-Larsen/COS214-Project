#include "../include/WorkflowInstance.h"
#include "../include/builder/WorkflowDefinition.h"
#include "../include/composite/WorkComponent.h"

#include <stdexcept>

WorkflowInstance::WorkflowInstance() : definition(NULL), root(NULL), createdAt(std::time(NULL)), status("Created"), routing(NULL)
{
}

WorkflowInstance::~WorkflowInstance()
{
	delete root;
}

void WorkflowInstance::buildFromDefinition(WorkflowDefinition *def)
{
	if(def == NULL || def->getRootTemplate() == NULL)
	{
		throw std::invalid_argument("definition has no root template");
	}

	WorkComponent *fresh = def->getRootTemplate()->clone(); // Prototype in action
	delete root;
	root = fresh;

	definition = def;
	status = "Running";
}

WorkComponent *WorkflowInstance::getRoot() const
{ 
	return root; 
}

void WorkflowInstance::progress()
{
	if(root)
	{
		root->progress();
	}
}

void WorkflowInstance::cancel()
{
	if(root)
	{
		root->cancel();
	}

	status = "Cancelled";
}

void WorkflowInstance::evaluateReadiness()
{
}

WorkComponent *WorkflowInstance::selectNextAvailable()
{
	return NULL;
}

Iterator *WorkflowInstance::createIterator()
{
	return root ? root->createIterator("dependency") : NULL;
}
