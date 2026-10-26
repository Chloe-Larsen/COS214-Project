#ifndef BLOCKED_H
#define BLOCKED_H

#include "State.h"

class Blocked : public State {
private:
	bool isEscalated;
public:
	Blocked();
	void progress(Item* item);
	void cancel(Item* item);
	void block(Item* item);
	void unblock(Item* item);
	bool getEscalated() override;
	void setEscalated() override;
	std::string getName() const override;
};

#endif
