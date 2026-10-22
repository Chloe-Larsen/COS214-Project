#ifndef CONDITIONAL_H
#define CONDITIONAL_H

#include "Routing.h"

class Conditional : Routing
{
public:
	Conditional();
	std::vector<WorkComponent *> nextWork(Item item, WorkflowInstance workflow) override;
};

#endif
