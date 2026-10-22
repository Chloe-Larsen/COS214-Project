#ifndef COMPLETED_H
#define COMPLETED_H

#include "State.h"

class Completed : State
{
public:
	Completed();
	void progress(Item *item);
	void cancel(Item *item);
	void block(Item *item);
	void unblock(Item *item);
};

#endif
