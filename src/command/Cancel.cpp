/**
 * @file Cancel.cpp
 * @brief Implementation of the Cancel command participant.
 * @ingroup tividy_command
 */
#include "../../include/command/Cancel.h"

Cancel::Cancel(Item *item) : WorkflowCommand(item)
{
}

void Cancel::execute()
{
    reciever->cancel();
}
std::string Cancel::getName()
{
    return "Cancel";
}