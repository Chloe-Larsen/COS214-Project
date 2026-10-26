#ifndef STATE_H
#define STATE_H

#include "../composite/Item.h"

class State
{
	public:
		State();
		virtual ~State();
		virtual void progress(Item *item) = 0;
		virtual void cancel(Item *item) = 0;
		virtual void block(Item *item) = 0;
		virtual void unblock(Item *item) = 0;
		virtual bool getEscalated() = 0;
		virtual void setEscalated() = 0;
		virtual std::string getName() = 0;
};

#endif