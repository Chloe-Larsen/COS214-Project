#ifndef WORKFLOWBUILDER_H
#define WORKFLOWBUILDER_H

#include "WorkflowDefinition.h"
class WorkflowBlueprint;
class WorkflowBuilder
{
protected:
	WorkflowDefinition *currentDefinition;

public:
	WorkflowBuilder();
	virtual ~WorkflowBuilder();
	virtual void reset() = 0;
	virtual void buildMetaData(std::string id, std::string name) = 0;
	virtual void buildParticipantsAndRoles(WorkflowBlueprint &blueprint) = 0;
	virtual void buildStagesAndItems(WorkflowBlueprint &blueprint) = 0;
	virtual void buildDependencies(WorkflowBlueprint &blueprint) = 0;
	virtual void buildRoutingStrategy(WorkflowBlueprint &blueprint) = 0;
	virtual void buildApprovalChains(WorkflowBlueprint &blueprint) = 0;
	virtual void buildExternalAdapters(WorkflowBlueprint &blueprint) = 0;
	virtual WorkflowDefinition *getResult() = 0;
};

#endif
