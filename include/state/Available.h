#ifndef AVAILABLE_H
#define AVAILABLE_H

#include "State.h"

class Available : State {
private:
	bool isEscalated;
public:
	Available();
	void progress(Item* item);
	void cancel(Item* item);
	void block(Item* item);
	void unblock(Item* item);
	void getEscalated(Item* item);
	void setEscalated(Item* item);
};

#endif
