#ifndef ASSIGN_H
#define ASSIGN_H

#include "WorkflowCommand.h"

class Assign : public WorkflowCommand
{
public:
	Assign(Item* item);
	Assign(Item *item, Worker *worker);
	void execute() override;
	std::string getName() override;
	private:
    Worker* worker;
};

#endif
