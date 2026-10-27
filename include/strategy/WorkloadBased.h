#ifndef WORKLOADBASED_H
#define WORKLOADBASED_H

#include "AssignmentStrategy.h"

class WorkloadBased : public AssignmentStrategy
{
public:
	WorkloadBased();
	Worker *assign(Item *item, WorkflowManager *manager) override;
};

#endif
