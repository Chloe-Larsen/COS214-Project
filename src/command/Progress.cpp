/**
 * @file Progress.cpp
 * @brief Implementation of the Progress command participant.
 * @ingroup tividy_command
 */

#include "../../include/command/Progress.h"

Progress::Progress(WorkComponent *workComponent) : workComponent(workComponent)
{
}

void Progress::execute()
{
    workComponent->progress();
}

std::string Progress::getName()
{
    return "Progress";
}