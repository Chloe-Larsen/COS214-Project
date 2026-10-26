#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include "../chain/Worker.h"

class AssignmentStrategy
{
public:
	AssignmentStrategy();
	virtual ~AssignmentStrategy();
	virtual Worker assign(Item item) = 0;
};

#endif
