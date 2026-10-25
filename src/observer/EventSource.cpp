#include "../../include/observer/EventSource.h"

#include <algorithm>

EventSource::EventSource()
{
	// TODO - implement EventSource::EventSource
	throw "Not yet implemented";
}

EventSource::EventSource(const EventSource &other)
{
	// TODO - implement EventSource::EventSource
	throw "Not yet implemented";
}

void EventSource::attach(Observer *observer)
{
	if(observer)
	{
		observers.push_back(observer);
	}
}

void EventSource::detach(Observer *observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void EventSource::notify(Event *event)
{
	for(size_t i = 0; i < observers.size(); ++i)
	{
		observers[i]->update(event);
	}
}
