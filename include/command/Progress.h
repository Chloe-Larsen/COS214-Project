/**
 * @file Progress.h
 * @brief Advance an item by one lifecycle transition.
 * @ingroup tividy_command
 */

#ifndef PROGRESS_H
#define PROGRESS_H

#include "WorkflowCommand.h"

/**
 * @brief Concrete Command participant for progressing an Item.
 * @details Makes exactly one call to Item::progress().
 * @ingroup tividy_command
 */
class Progress : public WorkflowCommand
{
private:
	/**
	 * @brief The work component to progress.
	 * @details The item must outlive this command and every call to execute().
	 */
	WorkComponent* workComponent;

public:
	/**
	 * @brief Bind the operation to an item.
	 * @param[in] workComponent Borrowed, non-null receiver that does not destruct with this command.
	 * @throws std::string If item is null.
	 */
	Progress(WorkComponent* workComponent);
	/**
	 * @brief Ask the receiver to perform one permitted state transition.
	 * @throws std::runtime_error If dependencies remain unresolved or the item is
	 * already COMPLETED or CANCELLED.
	 * @details State and observer exceptions propagate to the caller.
	 */
	void execute() override;
	/**
	 * @brief Return the operation name.
	 * @return "Progress".
	 */
	std::string getName() override;
};

#endif
