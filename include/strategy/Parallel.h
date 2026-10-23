#ifndef PARALLEL_H
#define PARALLEL_H

#include "Routing.h"

class Parallel : Routing
{
public:
	Parallel();
	std::vector<WorkComponent *> nextWork(Item item, WorkflowInstance workflow) override;
};

#endif
