#ifndef ESCALATE_H
#define ESCALATE_H

#include "WorkflowCommand.h"

class Escalate : public WorkflowCommand
{
public:
	Escalate();
	void execute() override;
	std::string getName() override;
};

#endif
