#ifndef GENERICWORKFLOWBUILDER_H
#define GENERICWORKFLOWBUILDER_H

#include "WorkflowBuilder.h"
#include "../composite/Item.h"
#include "../chain/Worker.h"
#include <unordered_map>

class GenericWorkflowBuilder : public WorkflowBuilder {

private:
	WorkflowBlueprint* blueprint;
	std::unordered_map<std::string, Item*> itemByName;
	std::unordered_map<std::string, Worker*> participantById;

public:
	GenericWorkflowBuilder();
	~GenericWorkflowBuilder();
	void reset() override;
	void buildMetaData(std::string id, std::string name) override;
	void buildParticipantsAndRoles(WorkflowBlueprint &blueprint) override;
	void buildStagesAndItems(WorkflowBlueprint &blueprint) override;
	void buildDependencies(WorkflowBlueprint &blueprint) override;
	void buildRoutingStrategy(WorkflowBlueprint &blueprint) override;
	void buildApprovalChains(WorkflowBlueprint &blueprint) override;
	void buildExternalAdapters(WorkflowBlueprint &blueprint) override;
	WorkflowDefinition *getResult() override;
};

#endif
