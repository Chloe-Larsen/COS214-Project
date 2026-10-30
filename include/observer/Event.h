#ifndef EVENT_H
#define EVENT_H

#include <string>

#include "EventType.h"

static const std::string KNOWN_EVENT_TYPES[] = {
	"COMPLETED",
	"APPROVED",
	"REJECTED",
	"ESCALATED",
	"ASSIGNED"};

static const std::size_t KNOWN_EVENT_TYPE_COUNT =
	sizeof(KNOWN_EVENT_TYPES) / sizeof(KNOWN_EVENT_TYPES[0]);

class Item;

class Event
{
private:
	std::string type;
	Item *source;
	std::string timestamp;

public:
	Event(std::string type, Item *source);
	std::string getType();
	static bool isValidType(const std::string &type);
	Item *getSource();
	std::string getTimestamp();
};

#endif
