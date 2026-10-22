#ifndef FILTERED_H
#define FILTERED_H

#include "Iterator.h"
#include "FilterCriteria.h"

class Filtered : public Iterator
{
private:
	Iterator *inner;
	FilterCriteria criteria;

public:
	Filtered(Iterator *inner, FilterCriteria c);
	~Filtered();
	virtual WorkComponent *first() override;
	virtual WorkComponent *next() override;
	virtual bool isDone() override;
	virtual WorkComponent *currentItem() override;
};

#endif
