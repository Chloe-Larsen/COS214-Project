#ifndef WORKFLOWMANAGER_H
#define WORKFLOWMANAGER_H

#include <ctime>
#include <string>
#include <vector>

#include "command/WorkflowCommand.h"
#include "composite/WorkComponent.h"
#include "strategy/Routing.h"
#include "strategy/AssignmentStrategy.h"
#include "factory/ItemBuilder.h"
#include "factory/ItemGroupBuilder.h"

class WorkflowManager {

private:
	std::string id;
	WorkComponent* root;
	std::time_t createdAt;
	std::string status;
	Routing* routing;
	AssignmentStrategy* assignmentStrategy;
	std::vector<WorkflowCommand*> history;

public:
	WorkflowManager();
	~WorkflowManager();
	void progress();
	void cancel();
	void assignWork(Item* item);
	Iterator createIterator();
	WorkComponent* selectNextAvailable();
	void executeCommand(WorkflowCommand* command);
	std::vector<WorkflowCommand*> getHistory();
	ItemBuilder* createItemBuilder();
	ItemGroupBuilder* createItemGroupBuilder();
};

#endif
