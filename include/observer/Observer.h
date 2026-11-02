#ifndef OBSERVER_H
#define OBSERVER_H

#include "Event.h"

class Observer
{
public:
	Observer();
	virtual ~Observer();

	/**
	 * @brief Update this observer so that it can appropriately handle the event.
	 *
	 * @param event The event to be handled by this event.
	 */
	virtual void update(Event *event) = 0;

	/**
	 * @brief Create a copy of this observer.
	 */
	virtual Observer *clone() = 0;
};

#endif
