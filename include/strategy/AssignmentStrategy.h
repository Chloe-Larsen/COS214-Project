#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include "../chain/Worker.h"
#include "../WorkflowInstance.h"

class AssignmentStrategy
{
public:
	AssignmentStrategy();
	virtual ~AssignmentStrategy();
	virtual Worker assign(Item item, WorkflowInstance instance) = 0;
};

#endif
