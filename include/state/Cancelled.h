#ifndef CANCELLED_H
#define CANCELLED_H

#include "State.h"

class Cancelled : State {
public:
	Cancelled();
	void progress(Item* item);
	void cancel(Item* item);
	void block(Item* item);
	void unblock(Item* item);
	std::string getName() const override;
};

#endif
