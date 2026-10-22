#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <ctime>
#include "builder/WorkflowDefinition.h"
#include "strategy/Routing.h"

class WorkflowInstance
{
private:
	std::string id;
	WorkflowDefinition *definition;
	WorkComponent *root;
	std::time_t createdAt;
	std::string status;
	Routing *routing;

public:
	WorkflowInstance();
	~WorkflowInstance();
	void buildFromDefinition(WorkflowDefinition *def);
	void evaluateReadiness();
	WorkComponent *selectNextAvailable();
	void progress();
	void cancel();
	Iterator createIterator();
};

#endif
