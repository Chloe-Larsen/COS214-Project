#include "../../include/observer/Dependency.h"
#include "../../include/composite/Item.h"

Dependency::Dependency(Item *item) : dependentItem(item)
{
	item->incrementDependencies();
}

void Dependency::update(Event *event)
{
	if (event->getType() == EventType::APPROVED)
		dependentItem->decrementDependencies();
}

Dependency *Dependency::clone()
{
	return nullptr; // do not clone dependencies when cloning an item
}