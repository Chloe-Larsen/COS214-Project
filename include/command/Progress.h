#ifndef PROGRESS_H
#define PROGRESS_H

#include "WorkflowCommand.h"

class Progress : public WorkflowCommand
{
public:
	Progress();
	void execute() override;
	std::string getName() override;
};

#endif
