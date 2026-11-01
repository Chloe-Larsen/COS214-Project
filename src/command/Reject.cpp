/**
 * @file Reject.cpp
 * @brief Implementation of the Reject command participant.
 * @ingroup tividy_command
 */

#include "../../include/command/Reject.h"

Reject::Reject(Item *item) : item(item), reason("Rejected by workflow command.")
{
}

Reject::Reject(Item *item, std::string reason) : item(item), reason(reason)
{
}

void Reject::execute()
{
    item->reject(reason);
}

std::string Reject::getName()
{
    return "Reject";
}