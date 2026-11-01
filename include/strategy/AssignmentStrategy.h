#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include "../chain/Worker.h"

class WorkflowManager;

class AssignmentStrategy
{
public:
	AssignmentStrategy();
	virtual ~AssignmentStrategy();

	/**
	 * @brief Select a worker for the item and assign it.
	 *
	 * @param item The item requiring an assignee.
	 * @param manager The manager supplying the worker pool.
	 * @return The assigned worker, or null if none is eligible.
	 */
	virtual Worker *assign(Item *item, WorkflowManager *manager) = 0;
};

#endif
