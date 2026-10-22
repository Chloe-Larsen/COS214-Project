#ifndef PRIORITYITERATOR_H
#define PRIORITYITERATOR_H

#include "Iterator.h"
#include <queue>

class PriorityIterator : Iterator
{
private:
	std::queue<WorkComponent *> queue;
public:	
	PriorityIterator(WorkComponent *root);
	~PriorityIterator();
	virtual WorkComponent *first() override;
	virtual WorkComponent *next() override;
	virtual bool isDone() override;
	virtual WorkComponent *currentItem() override;
};

#endif
