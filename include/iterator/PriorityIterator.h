#ifndef PRIORITYITERATOR_H
#define PRIORITYITERATOR_H

#include "Iterator.h"
#include <queue>

struct PriorityComparator
{
	bool operator()(WorkComponent *a, WorkComponent *b) const;
};

class PriorityIterator : public Iterator
{
private:
	std::priority_queue<WorkComponent *, std::vector<WorkComponent *>, PriorityComparator> queue;
	WorkComponent *current;

	void collectAll(WorkComponent *node);

public:
	PriorityIterator(WorkComponent *root);
	~PriorityIterator();
	virtual WorkComponent *first() override;
	virtual WorkComponent *next() override;
	virtual bool isDone() override;
	virtual WorkComponent *currentItem() override;
};

#endif
