#ifndef CONDITIONAL_H
#define CONDITIONAL_H

#include "Routing.h"

class Conditional : public Routing
{
public:
	Conditional();
	std::vector<WorkComponent *> nextWork(Item* item, WorkflowManager& manager) override;
};

#endif
