/**
 * @file Cancel.cpp
 * @brief Implementation of the Cancel command participant.
 * @ingroup tividy_command
 */
#include "../../include/command/Cancel.h"

Cancel::Cancel(WorkComponent *workComponent) : workComponent(workComponent)
{
}

void Cancel::execute()
{
    workComponent->cancel();
}

std::string Cancel::getName()
{
    return "Cancel";
}