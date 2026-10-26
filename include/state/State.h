#ifndef STATE_H
#define STATE_H

#include <string>

class Item;

class State
{
	public:
		State();
		virtual ~State();
		virtual void progress(Item *item) = 0;
		virtual void cancel(Item *item) = 0;
		virtual void block(Item *item) = 0;
		virtual void unblock(Item *item) = 0;
		virtual bool getEscalated() { return false;}
		virtual void setEscalated() {};
		virtual std::string getName() const = 0;
};

#endif