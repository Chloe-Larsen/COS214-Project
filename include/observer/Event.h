#ifndef EVENT_H
#define EVENT_H

#include "EventType.h"
#include <string>
#include "../composite/Item.h"

class Event
{
	private:
		EventType type;
		Item *source;
		std::string timestamp;

	public:
		EventType getType();
		Item *getSource();
		std::string getTimestamp();
};

#endif
