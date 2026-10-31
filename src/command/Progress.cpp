/**
 * @file Progress.cpp
 * @brief Implementation of the Progress command participant.
 * @ingroup tividy_command
 */

#include "../../include/command/Progress.h"

Progress::Progress(Item *item) : WorkflowCommand(item)
{
}

void Progress::execute()
{
    reciever->progress();
}

std::string Progress::getName()
{
    return "Progress";
}