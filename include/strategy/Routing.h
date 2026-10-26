#ifndef ROUTING_H
#define ROUTING_H

#include <vector>

#include "../composite/WorkComponent.h"
class WorkflowManager;

class Routing
{
public:
	Routing();
	virtual ~Routing();
	virtual std::vector<WorkComponent *> nextWork(Item* item, WorkflowManager& manager) = 0;
};

#endif
