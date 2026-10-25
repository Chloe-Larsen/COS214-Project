#ifndef OBSERVER_H
#define OBSERVER_H

#include "Event.h"
class Observer
{
public:
	Observer();
	virtual ~Observer();

	virtual void update(Event *event) = 0;
	virtual Observer *clone() = 0;
};

#endif
