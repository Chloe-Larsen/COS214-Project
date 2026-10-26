#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "Routing.h"

class Sequential : public Routing
{
public:
	Sequential();
	std::vector<WorkComponent *> nextWork(Item *item, WorkflowManager &manager) override;
};

#endif
