#include <chrono>
#include <iomanip>

#include "../../include/observer/Event.h"

std::string getTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

Event::Event(EventType type, Item *source) : type(type), source(source), timestamp(getTimestamp()) {}

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
