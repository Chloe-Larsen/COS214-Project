#ifndef RULEBASED_H
#define RULEBASED_H

#include "AssignmentStrategy.h"

class RuleBased : public AssignmentStrategy
{
public:
	RuleBased();
	Worker *assign(Item *item, WorkflowManager *manager) override;
};

#endif
