#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "Observer.h"

class Dependency : public Observer
{
private:
	Item *dependentItem;

public:
	Dependency(Item *item);

	/**
	 * @brief Decrement dependencies on this instance's dependent item if the item was approved.
	 * 
	 * @param event The event used to check if the item was approved.
	 */
	void update(Event *event) override;

	Dependency *clone() override;
};

#endif
