#include "../../include/iterator/Filtered.h"
#include "../../include/composite/ItemGroup.h"

Filtered::Filtered(Iterator *inner, FilterCriteria c) : Iterator(nullptr), inner(inner), criteria(c), current(nullptr)
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
	ItemGroup *group = dynamic_cast<ItemGroup *>(component);

	if (criteria.getType() == ItemType::ITEM && !item)
	{
		return false;
	}
	if (criteria.getType() == ItemType::ITEM_GROUP && !group)
	{
		return false;
	}

	if (criteria.getStatus() != ItemStatus::ANY)
	{
		std::string expectedStatus;
		switch (criteria.getStatus())
		{
		case ItemStatus::AVAILABLE:
			expectedStatus = "AVAILABLE";
			break;
		case ItemStatus::IN_PROGRESS:
			expectedStatus = "IN_PROGRESS";
			break;
		case ItemStatus::COMPLETED:
			expectedStatus = "COMPLETED";
			break;
		case ItemStatus::REJECTED:
			expectedStatus = "REJECTED";
			break;
		case ItemStatus::BLOCKED:
			expectedStatus = "BLOCKED";
			break;
		default:
			break;
		}
		if (!expectedStatus.empty() && component->getStatus() != expectedStatus)
		{
			return false;
		}
	}

	if (criteria.getAssignedTo() != nullptr)
	{
		if (!item || item->getWorker() != criteria.getAssignedTo())
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