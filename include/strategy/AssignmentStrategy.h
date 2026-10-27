#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include "../chain/Worker.h"

class WorkflowManager;

class AssignmentStrategy
{
public:
	AssignmentStrategy();
	virtual ~AssignmentStrategy();
	virtual Worker *assign(Item *item, WorkflowManager *manager) = 0;
};

#endif
