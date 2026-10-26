#ifndef INPROGRESS_H
#define INPROGRESS_H

#include "State.h"

class InProgress : public State
{
private:
	bool isEscalated;
public:
	InProgress();
	void progress(Item *item);
	void cancel(Item *item);
	void block(Item *item);
	void unblock(Item *item);
	bool getEscalated() override;
	void setEscalated() override;
	std::string getName() const override;
};

#endif
