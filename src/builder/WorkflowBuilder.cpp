#include "../../include/builder/WorkflowBuilder.h"

WorkflowBuilder::WorkflowBuilder() : currentDefinition(nullptr)
{
}

WorkflowBuilder::~WorkflowBuilder()
{
	if (currentDefinition)
		delete currentDefinition;
}