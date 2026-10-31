#include "../../include/iterator/PriorityIterator.h"
#include "../../include/composite/ItemGroup.h"
#include "../../include/composite/Item.h"

/**
 * @brief Compares two components by priority
 * @param a First component
 * @param b Second component
 * @return true if a has lower priority than b
 */
bool PriorityComparator::operator()(WorkComponent *a, WorkComponent *b) const
{
	int prioA = 0;
	int prioB = 0;

	Item *itemA = dynamic_cast<Item *>(a);
	Item *itemB = dynamic_cast<Item *>(b);

	if (itemA)
		prioA = itemA->getPriority();
	if (itemB)
		prioB = itemB->getPriority();

	return prioA < prioB;
}

/**
 * @brief Constructs a PriorityIterator rooted at the given component
 * @param root Root of the component tree to traverse
 */
PriorityIterator::PriorityIterator(WorkComponent *root) : Iterator(root), current(nullptr)
{
	first();
}

/** @brief Deconstructor */
PriorityIterator::~PriorityIterator()
{
	if (current)
		delete current;
}

/**
 * @brief Recursively collects all components in the subtree into the queue
 * @param node Root of the subtree to collect
 */
void PriorityIterator::collectAll(WorkComponent *node)
{
	if (!node)
		return;
	queue.push(node);

	int count = node->getChildCount();
	for (int i = 0; i < count; ++i)
	{
		ItemGroup *group = dynamic_cast<ItemGroup *>(node);
		if (group)
		{
			WorkComponent *child = group->getChild(i);
			if (child)
				collectAll(child);
		}
	}
}

/**
 * @brief Resets the iterator and returns the highest-priority component
 * @return The first component in priority order, or nullptr if empty
 */
WorkComponent *PriorityIterator::first()
{
	while (!queue.empty())
	{
		queue.pop();
	}
	current = nullptr;

	if (root)
	{
		collectAll(root);
		return next();
	}
	return nullptr;
}

/**
 * @brief  Advances to and returns the next highest-priority component
 * @return The next component, or nullptr if traversal is completed
 */
WorkComponent *PriorityIterator::next()
{
	if (queue.empty())
	{
		current = nullptr;
		return nullptr;
	}

	current = queue.top();
	queue.pop();
	return current;
}

/**
 * @brief Checks whether the priority traversal has finished
 * @return true if no current item and queue is empty
 */
bool PriorityIterator::isDone()
{
	return current == nullptr && queue.empty();
}

/**
 * @brief Returns the current commonent without updating
 * @return The current component, nullptr if none
 */
WorkComponent *PriorityIterator::currentItem()
{
	return current;
}