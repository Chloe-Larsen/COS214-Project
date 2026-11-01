#ifndef WORKLOADBASED_H
#define WORKLOADBASED_H

#include "AssignmentStrategy.h"

class WorkloadBased : public AssignmentStrategy
{
public:
	WorkloadBased();

	/**
	 * @brief Assign the item to the least-loaded worker.
	 *
	 * @param item The item requiring an assignee.
	 * @param manager The manager supplying the worker pool.
	 * @return The assigned worker, or null if the pool is empty.
	 */
	Worker *assign(Item *item, WorkflowManager *manager) override;
};

#endif
