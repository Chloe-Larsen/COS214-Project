#ifndef INPROGRESS_H
#define INPROGRESS_H

#include "State.h"

class InProgress : State
{
private:
	bool isEscalted;
public:
	InProgress();
	void progress(Item *item);
	void cancel(Item *item);
	void block(Item *item);
	void unblock(Item *item);
	void getEscalated(Item *item);
	void setEscalated(Item *item);
	std::string getName() const override;
};

#endif
