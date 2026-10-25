#include "../../include/builder/GenericWorkflowBuilder.h"
#include "../../include/composite/ItemGroup.h"

GenericWorkflowBuilder::GenericWorkflowBuilder() : WorkflowBuilder(), blueprint(nullptr)
{
}

GenericWorkflowBuilder::~GenericWorkflowBuilder()
{
	reset();
}

void GenericWorkflowBuilder::reset()
{
	currentDefinition = nullptr;
	blueprint = nullptr;
	itemByName.clear();
	participantById.clear();
}

void GenericWorkflowBuilder::buildMetaData(std::string id, std::string name)
{
	if (currentDefinition)
	{
		delete currentDefinition;
	}
	currentDefinition = new WorkflowDefinition(id, name);
}

void GenericWorkflowBuilder::buildParticipantsAndRoles(WorkflowBlueprint &blueprint)
{
	this->blueprint = &blueprint;
	for (const Worker &workerData : blueprint.getWorkers())
	{
		participantById[workerData.getId()] = new Worker(workerData.getId(), workerData.getName(), workerData.getRole(), workerData.getSupervisor());
	}
}

void GenericWorkflowBuilder::buildStagesAndItems(WorkflowBlueprint &blueprint)
{
	if (!currentDefinition)
		return;

	ItemGroup *rootGroup = new ItemGroup("ROOT_STAGE", "Root Stage", "AVAILABLE");
	for (const Item &itemData : blueprint.getItems())
	{
		Item *newItem = new Item(itemData);
		itemByName[itemData.getName()] = newItem;
		rootGroup->add(newItem);
	}
	currentDefinition->setRootTemplate(rootGroup);
}

void GenericWorkflowBuilder::buildDependencies(WorkflowBlueprint &blueprint)
{
	for (const auto &dep : blueprint.getDependencies())
	{
		Item *prereq = itemByName[dep.first];
		Item *target = itemByName[dep.second];
		if (prereq && target)
		{
			target->incrementDependencies();
		}
	}
}

void GenericWorkflowBuilder::buildRoutingStrategy(WorkflowBlueprint &blueprint)
{
}

void GenericWorkflowBuilder::buildApprovalChains(WorkflowBlueprint &blueprint)
{
}

void GenericWorkflowBuilder::buildExternalAdapters(WorkflowBlueprint &blueprint)
{
}

WorkflowDefinition *GenericWorkflowBuilder::getResult()
{
	WorkflowDefinition* product = currentDefinition;
    currentDefinition = nullptr;
    reset();
    return product;
}