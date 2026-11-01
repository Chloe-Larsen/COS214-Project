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
private:
	/**
	 * @brief The work component to cancel.
	 * @details The item must outlive this command and every call to execute().
	 */
	WorkComponent* workComponent;

public:
	/**
	 * @brief Bind the operation to a work component.
	 * @param[in] workComponent Borrowed, non-null receiver that must outlive this command.
	 * @throws std::string If workComponent is null.
	 */
	Cancel(WorkComponent* workComponent);
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
