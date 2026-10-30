#include <chrono>
#include <sstream>
#include <iomanip>

#include "../../include/observer/Event.h"

const std::string KNOWN_EVENT_TYPES[] = {
	"COMPLETED",
	"APPROVED",
	"REJECTED",
	"ESCALATED",
	"ASSIGNED"};

const std::size_t KNOWN_EVENT_TYPE_COUNT =
	sizeof(KNOWN_EVENT_TYPES) / sizeof(KNOWN_EVENT_TYPES[0]);

bool Event::isValidType(const std::string &type)
{
	for (std::size_t i = 0; i < KNOWN_EVENT_TYPE_COUNT; ++i)
		if (KNOWN_EVENT_TYPES[i] == type)
			return true;
	return false;
}

std::string getTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

Event::Event(std::string type, Item *source) : type(type), source(source), timestamp(getTimestamp())
{
	if (!isValidType(type))
		throw std::string("Unknown event type: " + type);
}

std::string Event::getType()
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
