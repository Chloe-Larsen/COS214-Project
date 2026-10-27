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
