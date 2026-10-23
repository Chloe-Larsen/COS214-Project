#include "../../include/observer/Event.h"

EventType Event::getType()
{
	return this->type;
}

Item *Event::getSource()
{
	return this->source;
}

std::string Event::getTimestamp()
{
	return this->timestamp;
}
