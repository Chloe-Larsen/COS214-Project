#ifndef ROUTING_H
#define ROUTING_H

#include <vector>

#include "../WorkflowInstance.h"
#include "../composite/WorkComponent.h"

class Routing
{
public:
	Routing();
	virtual ~Routing();
	virtual std::vector<WorkComponent *> nextWork(Item item, WorkflowInstance workflow) = 0;
};

#endif
