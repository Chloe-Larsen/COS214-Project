#ifndef BLOCKED_H
#define BLOCKED_H

#include "State.h"

class Blocked : State {
private:
	bool isEscalated;
public:
	Blocked();
	void progress(Item* item);
	void cancel(Item* item);
	void block(Item* item);
	void unblock(Item* item);
	void getEscalated(Item* item);
	void setEscalated(Item* item);
	std::string getName() const override;
};

#endif
