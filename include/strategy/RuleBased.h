#ifndef RULEBASED_H
#define RULEBASED_H

#include "AssignmentStrategy.h"

class RuleBased : AssignmentStrategy
{
public:
	RuleBased();
	Worker assign(Item item, WorkflowInstance instance) override;
};

#endif
