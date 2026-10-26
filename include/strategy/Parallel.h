#ifndef PARALLEL_H
#define PARALLEL_H

#include "Routing.h"

class Parallel : public Routing
{
public:
	Parallel();
	std::vector<WorkComponent *> nextWork(Item *item, WorkflowManager &manager) override;
};

#endif
