#ifndef EVENT_H
#define EVENT_H

#include <string>

#include "EventType.h"

class Item;

class Event
{
private:
	EventType type;
	Item *source;
	std::string timestamp;

public:
	Event(EventType type, Item *source);
	EventType getType();
	Item *getSource();
	std::string getTimestamp();
};

#endif
