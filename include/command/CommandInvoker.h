#ifndef COMMANDINVOKER_H
#define COMMANDINVOKER_H

#include <vector>
#include "WorkflowCommand.h"

class CommandInvoker {
private:
	std::vector<WorkflowCommand*> history;
public:
	CommandInvoker();
	void executeCommand(WorkflowCommand* command);
	std::vector<WorkflowCommand*> getHistory();
};

#endif
