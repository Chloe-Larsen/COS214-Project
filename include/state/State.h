#ifndef STATE_H
#define STATE_H

#include "../composite/Item.h"

class State
{
public:
	State();
	virtual ~State() = 0;
	virtual void progress(Item *item) = 0;
	virtual void cancel(Item *item) = 0;
	virtual void block(Item *item) = 0;
	virtual void unblock(Item *item) = 0;
	bool getEscalated();
	void setEscalated();
};

#endif
