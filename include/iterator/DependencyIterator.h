#ifndef DEPENDENCYITERATOR_H
#define DEPENDENCYITERATOR_H

#include "Iterator.h"
#include <stack>

class DependencyIterator : public Iterator {
private:
	std::stack<WorkComponent*> stack;
public:
	DependencyIterator(WorkComponent* root);
	~DependencyIterator();
	virtual WorkComponent *first() override;
	virtual WorkComponent *next() override;
	virtual bool isDone() override;
	virtual WorkComponent *currentItem() override;
};

#endif
