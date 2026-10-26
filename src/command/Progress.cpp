#include "../../include/command/Progress.h"

Progress::Progress(Item *item) : WorkflowCommand(item) {}
void Progress::execute() { reciever->progress(); }
std::string Progress::getName() { return "Progress"; }