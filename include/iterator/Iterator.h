#ifndef ITERATOR_H
#define ITERATOR_H

class WorkComponent;

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