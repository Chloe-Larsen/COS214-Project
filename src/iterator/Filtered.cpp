#include "../../include/iterator/Filtered.h"
#include "../../include/composite/ItemGroup.h"

Filtered::Filtered(Iterator *inner, FilterCriteria *c) : Iterator(nullptr), inner(inner), criteria(c), current(nullptr)
{
	first();
}

Filtered::~Filtered()
{
	if (inner)
		delete inner;
}

bool Filtered::matches(WorkComponent *component) const
{
	if (!component)
		return false;

	Item *item = dynamic_cast<Item *>(component);

	if (criteria->hasStatusFilter())
	{
		if (!item)
			return false;
		if (!criteria->allowsStatus(item->getStatus()))
			return false;
	}

	if (criteria->getAssignedTo() != nullptr)
	{
		if (!item || item->getWorker() != criteria->getAssignedTo())
		{
			return false;
		}
	}

	return true;
}

void Filtered::advanceToNextMatch()
{
	current = nullptr;

	while (inner && !inner->isDone())
	{
		WorkComponent *candidate = inner->currentItem();
		inner->next();
		if (matches(candidate))
		{
			current = candidate;
			break;
		}
	}
}

WorkComponent *Filtered::first()
{
	if (!inner)
		return nullptr;

	inner->first();
	advanceToNextMatch();
	return current;
}

WorkComponent *Filtered::next()
{
	advanceToNextMatch();
	return current;
}

bool Filtered::isDone()
{
	return current == nullptr;
}

WorkComponent *Filtered::currentItem()
{
	return current;
}