#ifndef AVAILABLE_H
#define AVAILABLE_H

#include "State.h"

class Available : public State {
private:
	bool isEscalated;
public:
	Available();
	void progress(Item* item);
	void cancel(Item* item);
	void block(Item* item);
	void unblock(Item* item);
	bool getEscalated() override;
	void setEscalated() override;
	std::string getName() const override;
};

#endif
