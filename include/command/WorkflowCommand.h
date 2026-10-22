#ifndef WORKFLOWCOMMAND_H
#define WORKFLOWCOMMAND_H
#include "../composite/Item.h"
#include <string>
class WorkflowCommand
{
protected:
	Item *reciever;

public:
	WorkflowCommand(Item *reciever);
	virtual void execute() = 0;
	virtual std::string getName() = 0;
};

#endif
