/**
 * @file WorkflowCommand.h
 * @brief Abstract command interface for item operations.
  * @ingroup tividy_command
 */

#ifndef WORKFLOWCOMMAND_H
#define WORKFLOWCOMMAND_H
#include "../composite/Item.h"
#include <string>
/**
 * @brief Abstract Command participant for an operation on one Item.
 * @details Concrete commands retain their request data and transfer execution to
 * the receiver.
 * @ingroup tividy_command
 */
class WorkflowCommand
{
protected:
	

public:
	/**
	 * @brief Create a new command.
	 */
	WorkflowCommand();
	/**
	 * @brief Destroy the command without deleting its receiver.
	 * @details Virtual destruction supports ownership through WorkflowCommand pointers.
	 */
	virtual ~WorkflowCommand();
	/**
	 * @brief Execute the concrete operation on the receiver.
	 * @details Validation and state changes belong to the receiver and its associated
	 * core objects. Exceptions from the concrete operation return to the caller.
	 * @pre The borrowed receiver and any other borrowed arguments are still alive.
	 */
	virtual void execute() = 0;
	/**
	 * @brief Return the command's operation name.
	 * @return The concrete command name, such as "Assign" or "Progress".
	 */
	virtual std::string getName() = 0;
};

#endif
