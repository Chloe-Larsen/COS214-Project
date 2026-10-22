#ifndef WORKLOADBASED_H
#define WORKLOADBASED_H

#include "AssignmentStrategy.h"

class WorkloadBased : AssignmentStrategy
{
public:
	WorkloadBased();
	Worker assign(Item item, WorkflowInstance instance) override;
};

#endif
