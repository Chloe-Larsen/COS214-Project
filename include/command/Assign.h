#ifndef ASSIGN_H
#define ASSIGN_H

#include "WorkflowCommand.h"

class Assign : public WorkflowCommand
{
public:
	Assign();
	void execute() override;
	std::string getName() override;
};

#endif
