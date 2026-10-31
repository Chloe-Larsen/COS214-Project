/**
 * @file Escalate.h
 * @brief Declares the command for escalating an item.
 * @ingroup tividy_command
 */

#ifndef ESCALATE_H
#define ESCALATE_H

#include "WorkflowCommand.h"

/**
 * @brief Encapsulates an item escalation request.
 * @details Transfers execution to the item's escalation operation.
 * @ingroup tividy_command
 */
class Escalate : public WorkflowCommand
{
public:
    /**
     * @brief Bind the escalation command to an item.
     * @param[in] item Borrowed receiver that must outlive this command.
     * @throws std::string If item is null.
     */
    Escalate(Item* item);

    /**
     * @brief Request escalation of the receiver.
     * @details Calls Item::escalate() and receiver's handle exceptions.
     */
    void execute() override;

    /**
     * @brief Return the command name.
     * @return "Escalate".
     */
    std::string getName() override;
};

#endif