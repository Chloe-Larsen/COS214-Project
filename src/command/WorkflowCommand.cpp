/**
 * @file WorkflowCommand.cpp
 * @brief Implementation of the WorkflowCommand command participant.
  * @ingroup tividy_command
 */
#include "../../include/command/WorkflowCommand.h"

WorkflowCommand::WorkflowCommand(Item* reciever): reciever(reciever)
{
    if (reciever == nullptr)
    {
        throw std::string("A command needs an Item receiver.");
    }
}

WorkflowCommand::~WorkflowCommand()
{
}