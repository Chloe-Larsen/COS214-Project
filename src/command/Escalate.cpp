/**
 * @file Escalate.cpp
 * @brief Implementation of the Escalate command participant.
 * @ingroup tividy_command
 */
#include "../../include/command/Escalate.h"

Escalate::Escalate(Item *item) : WorkflowCommand(item)
{
}

void Escalate::execute()
{
    reciever->escalate();
}

std::string Escalate::getName()
{
    return "Escalate";
}
