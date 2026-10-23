#ifndef APPROVED_H
#define APPROVED_H

#include "State.h"

class Approved : State
{
public : Approved();
	void progress(Item *item);
	void cancel(Item *item);
	void block(Item *item);
	void unblock(Item *item);
};

#endif
