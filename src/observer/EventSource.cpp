#include <algorithm>

#include "../../include/observer/EventSource.h"

EventSource::EventSource() {}

EventSource::EventSource(const EventSource &other)
{
	for (Observer *observer : other.observers)
	{
		Observer *clone = observer->clone();

		if (clone != nullptr)
			attach(clone);
	}
}

EventSource::~EventSource() {}

void EventSource::attach(Observer *observer)
{
	observers.push_back(observer);
}

void EventSource::detach(Observer *observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void EventSource::notify(Event *event)
{
	for (Observer *observer : observers)
		observer->update(event);
}