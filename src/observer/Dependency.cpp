#include "../../include/observer/Dependency.h"

Dependency::Dependency(Item *item) : dependentItem(item) {}

void Dependency::update(Event *event)
{
	if (event->getType() == EventType::APPROVED)
		dependentItem->decrementDependencies();
}

Dependency *Dependency::clone()
{
	return new Dependency(dependentItem);
}