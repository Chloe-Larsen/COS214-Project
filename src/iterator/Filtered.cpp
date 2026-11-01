#include "../../include/iterator/Filtered.h"
#include "../../include/composite/ItemGroup.h"

/**
 * @brief Constructs a Filtered iterator
 * @param inner Inner iterator to wrap (ownership transferred)
 * @param c FilterCriteria to apply (not owned)
 */
Filtered::Filtered(Iterator *inner, FilterCriteria *c) : Iterator(nullptr), inner(inner), criteria(c), current(nullptr)
{
	first();
}

/** @brief Destructor Deletes the inner iterator */
Filtered::~Filtered()
{
	if (inner)
		delete inner;
}

/**
 * @brief Determines whether a component satisfies the criteria
 * @param component Candidate component to test
 * @return true if the component matches; false otherwise
 */
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

/**
 * @brief Advances the inner iterator to the next matching component
 *
 * Sets current to the next matching component, or nullptr if none remain
 */
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

/**
 * @brief Resets and returns the first matching component
 * @return The first matching component, or nullptr if none
 */
WorkComponent *Filtered::first()
{
	if (!inner)
		return nullptr;

	inner->first();
	advanceToNextMatch();
	return current;
}

/**
 * @brief Advances to and returns the next matching component
 * @return The next matching component, or nullptr if none
 */
WorkComponent *Filtered::next()
{
	advanceToNextMatch();
	return current;
}

/**
 * @brief Checks whether the filtered traversal has finished
 * @return true if no current matching component remains
 */
bool Filtered::isDone()
{
	return current == nullptr;
}

/**
 * @brief Returns the current matching component
 * @return The current component, or nullptr if none
 */
WorkComponent *Filtered::currentItem()
{
	return current;
}