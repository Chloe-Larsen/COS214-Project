#include "../../include/command/Reject.h"

Reject::Reject(Item *item)
    : WorkflowCommand(item), reason("Rejected by workflow command.") {}
Reject::Reject(Item *item, std::string reason)
    : WorkflowCommand(item), reason(reason) {}
void Reject::execute() { reciever->reject(reason); }
std::string Reject::getName() { return "Reject"; }