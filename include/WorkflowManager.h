#ifndef WORKFLOWMANAGER_H
#define WORKFLOWMANAGER_H

#include <ctime>
#include <string>
#include <vector>

#include "command/WorkflowCommand.h"
#include "iterator/FilterCriteria.h"
#include "composite/WorkComponent.h"
#include "strategy/Routing.h"
#include "strategy/AssignmentStrategy.h"
#include "factory/ItemBuilder.h"
#include "factory/ItemGroupBuilder.h"

class WorkflowManager
{
private:
	WorkComponent *root;
	std::time_t createdAt;
	Routing *routing;
	AssignmentStrategy *assignmentStrategy;
	std::vector<WorkflowCommand *> commandHistory;
	std::vector<Worker *> workers;

	WorkflowManager(const WorkflowManager &) = delete;
	WorkflowManager &operator=(const WorkflowManager &) = delete;

public:
	WorkflowManager();
	~WorkflowManager();

	void progress();
	void cancel();
	void assignWork(Item *item);

	Iterator *createDependencyIterator();
	Iterator *createPriorityIterator();
	Iterator *createFilteredIterator(Iterator *iterator, FilterCriteria *criteria);

	WorkComponent *selectNextAvailable();
	void executeCommand(WorkflowCommand *command);
	std::vector<WorkflowCommand *> getHistory();
	std::vector<Worker *> getWorkers();
	void addWorker(Worker *worker);
	ItemBuilder *createItemBuilder();
	ItemGroupBuilder *createItemGroupBuilder();
};

#endif
