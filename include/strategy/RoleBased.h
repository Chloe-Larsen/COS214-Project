#ifndef ROLEBASED_H
#define ROLEBASED_H

#include "AssignmentStrategy.h"

class RoleBased : public AssignmentStrategy
{
private:
	std::string role;

public:
	/**
	 * @brief Construct a role-based strategy.
	 *
	 * @param role The role a worker must hold to be eligible.
	 */
	RoleBased(const std::string &role);

	/**
	 * @brief Assign the item to the least-loaded worker with the configured role.
	 *
	 * @param item The item requiring an assignee.
	 * @param manager The manager supplying the worker pool.
	 * @return The assigned worker, or null if no worker has the role.
	 */
	Worker *assign(Item *item, WorkflowManager *manager) override;
};

#endif
