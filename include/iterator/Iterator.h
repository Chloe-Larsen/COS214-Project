#ifndef ITERATOR_H
#define ITERATOR_H

#include "../composite/WorkComponent.h"
class Iterator
{
protected:
	WorkComponent *root;

public:
	Iterator(WorkComponent *root);
	virtual ~Iterator();
	virtual WorkComponent *first() = 0;
	virtual WorkComponent *next() = 0;
	virtual bool isDone() = 0;
	virtual WorkComponent *currentItem() = 0;
};

#endif
