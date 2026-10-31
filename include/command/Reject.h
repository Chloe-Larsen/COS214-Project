/**
 * @file Reject.h
 * @brief Record an item rejection through a command.
 * @ingroup tividy_command
 */

#ifndef REJECT_H
#define REJECT_H

#include "WorkflowCommand.h"

/**
 * @brief Concrete Command participant for recording an Item rejection.
 * @details Owns the rejection text and borrows the receiver. Item::reject()
 * validates the reason and publishes the rejection event.
 * @ingroup tividy_command
 */
class Reject : public WorkflowCommand {
private:
	/**
	 * @brief Owned rejection reason passed to the receiver.
	 */
	std::string reason;
public:
	/**
	 * @brief Construct a rejection using the default reason.
	 * @param[in] item Borrowed, non-null receiver that does not destruct with the command.
	 * @throws std::string If item is null.
	 * @details The default text is "Rejected by workflow command.".
	 */
	Reject(Item* item);
	/**
	 * @brief Construct a rejection with caller-provided text.
	 * @param[in] item Borrowed, non-null receiver tthat does not destruct with the command.
	 * @param[in] reason Rejection text, copied into this command.
	 * @throws std::string If item is null.
	 * @note Reason validation happens in execute()
	 */
	Reject(Item* item, std::string reason);
	/**
	 * @brief Record the captured reason on the receiver.
	 * @throws std::invalid_argument If the reason is empty or whitespace-only.
	 * @details Sets the rejection flag and publishes REJECTED.
	 */
	void execute() override;
	/**
	 * @brief Return the operation name.
	 * @return "Reject".
	 */
	std::string getName() override;
};

#endif
