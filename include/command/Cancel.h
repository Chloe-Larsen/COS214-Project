#ifndef CANCEL_H
#define CANCEL_H

#include "WorkflowCommand.h"

class Cancel : public WorkflowCommand
{
public:
	Cancel(Item* item);
	void execute() override;
	std::string getName() override;
};

#endif
