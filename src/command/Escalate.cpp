/**
 * @file Escalate.cpp
 * @brief Implementation of the Escalate command participant.
 * @ingroup tividy_command
 */
#include "../../include/command/Escalate.h"

Escalate::Escalate(Item *item) : item(item)
{
}

void Escalate::execute()
{
    item->escalate();
}

std::string Escalate::getName()
{
    return "Escalate";
}
