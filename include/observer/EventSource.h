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
	EventSource(const EventSource &other);
	virtual ~EventSource();

	/**
	 * @brief Register an observer so that it is updated when this event source is notified.
	 * 
	 * @param observer The observer to register.
	 */
	void attach(Observer *observer);

	/**
	 * @brief Unregister an observer so that it is no longer updated when this event source is notified.
	 * 
	 * @param observer The observer to unregister.
	 */
	void detach(Observer *observer);

	/**
	 * @brief Trigger an update that is forwarded to each observer registered to this event source.
	 * 
	 * @param observer The event to be forwarded to each observer.
	 */
	void notify(Event *event);
};

#endif
