#ifndef ESCALATE_H
#define ESCALATE_H

#include "WorkflowCommand.h"

class Escalate : public WorkflowCommand
{
public:
	Escalate(Item* item);
	void execute() override;
	std::string getName() override;
};

#endif
