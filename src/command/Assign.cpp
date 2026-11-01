/**
 * @file Assign.cpp
 * @brief Implementation of the Assign command participant.
 * @ingroup tividy_command
 */

#include "../../include/command/Assign.h"

Assign::Assign(Item *item) : item(item), worker(item->getWorker())
{
}

Assign::Assign(Item *item, Worker *worker) : item(item), worker(worker)
{
    if (!worker)
        throw std::string("Assign needs a non-null Worker.");
}

void Assign::execute()
{
    if (!worker)
        throw std::string("Supply a Worker when assigning an unassigned Item.");
    item->assignWorker(worker);
}

std::string Assign::getName()
{
    return "Assign";
}