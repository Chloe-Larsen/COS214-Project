/**
 * @file Cancel.h
 * @brief Cancel an item through its current lifecycle state.
  * @ingroup tividy_command
 */

#ifndef CANCEL_H
#define CANCEL_H

#include "WorkflowCommand.h"

/**
 * @brief Concrete Command participant for cancelling an Item.
 * @details Lifecycle validation and cancellation calls Item::cancel().
 * @ingroup tividy_command
 */
class Cancel : public WorkflowCommand
{
public:
	/**
	 * @brief Bind the operation to an item.
	 * @param[in] item Borrowed, non-null receiver that must outlive this command.
	 * @throws std::string If item is null.
	 */
	Cancel(Item* item);
	/**
	 * @brief Cancel the receiver through its current State.
	 * @throws std::runtime_error If the item is already COMPLETED or CANCELLED.
	 * @details Receiver and observer exceptions propagate to the caller.
	 */
	void execute() override;
	/**
	 * @brief Return the operation name.
	 * @return "Cancel".
	 */
	std::string getName() override;
};

#endif
