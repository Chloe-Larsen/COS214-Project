#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "Routing.h"

class Sequential : Routing
{
public:
	Sequential();
	std::vector<WorkComponent *> nextWork(Item item, WorkflowInstance workflow) override;
};

#endif
