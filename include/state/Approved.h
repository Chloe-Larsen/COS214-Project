#ifndef APPROVED_H
#define APPROVED_H

#include "State.h"

class Approved : public State
{
public : Approved();
	void progress(Item *item);
	void cancel(Item *item);
	void block(Item *item);
	void unblock(Item *item);
	std::string getName() const override;
};

#endif
