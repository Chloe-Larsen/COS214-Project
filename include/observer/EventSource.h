#ifndef EVENTSOURCE_H
#define EVENTSOURCE_H

#include <string>
#include <vector>
#include "Observer.h"
#include "Event.h"

class EventSource
{
private:
	std::vector<Observer *> observers;

public:
	EventSource();
	EventSource(EventSource &other);
	virtual ~EventSource();

	void attach(Observer *observer);
	void detach(Observer *observer);
	void notify(Event *event);
};

#endif
