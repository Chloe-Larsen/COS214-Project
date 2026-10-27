#include "../../include/command/Cancel.h"

Cancel::Cancel(Item *item) : WorkflowCommand(item)
{
}

void Cancel::execute()
{
    reciever->cancel();
}
std::string Cancel::getName()
{
    return "Cancel";
}