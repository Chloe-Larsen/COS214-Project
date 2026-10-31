/**
 * @file Assign.h
 * @brief Command that assigns a worker to an item.
 * @ingroup tividy_command
 */

#ifndef ASSIGN_H
#define ASSIGN_H

#include "WorkflowCommand.h"

/**
 * @brief Concrete Command participant for assigning one worker to an Item.
 * @details The worker and receiver are borrowed. A successful execute() calls
 * Item::assignWorker(), which creates an assignment event when it changes.
 * @ingroup tividy_command
 */
class Assign : public WorkflowCommand
{
public:
	/**
	 * @brief Capture the item's current worker as the assignment target.
	 * @param[in] item Borrowed, non-null receiver.
	 * @throws std::string If item is null.
	 * @note The target is captured during construction. If the item is unassigned,
	 * construction succeeds but execute() rejects the missing worker.
	 */
	Assign(Item* item);
	/**
	 * @brief Capture an explicitly selected worker.
	 * @param[in] item Borrowed, non-null receiver.
	 * @param[in] worker Borrowed, non-null worker to assign.
	 * @throws std::string If either argument is null.
	 * @pre Both objects outlive this command.
	 */
	Assign(Item *item, Worker *worker);
	/**
	 * @brief Assign the captured worker to the receiver.
	 * @throws std::string If no worker was captured.
	 * @details Replaces the receiver's single assignee. An unchanged assignment
	 * produces no duplicate assignment event. Observer exceptions propagate.
	 */
	void execute() override;
	/**
	 * @brief Return the operation name.
	 * @return "Assign".
	 */
	std::string getName() override;
	private:
    /**
     * @brief Borrowed worker captured when this command is constructed.
     */
    Worker* worker;
};

#endif
