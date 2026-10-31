#include "../../include/iterator/DependencyIterator.h"
#include "../../include/composite/ItemGroup.h"

/**
 * @brief Constructs a DependencyIterator rooted at the given component
 * @param root The root of the component tree to traverse
 *
 * The constructor initializes the traversal by invoking first()
 */
DependencyIterator::DependencyIterator(WorkComponent *root) : Iterator(root), current(nullptr)
{
	first();
}

/** @brief Destructor */
DependencyIterator::~DependencyIterator()
{
	if (current != nullptr)
		delete current;
}

/**
 * @brief Pushes the children of the given component onto the stack
 * @param component The component whose children should be pushed
 *
 * Children are pushed in reverse order so that the first child is popped (and thus visited) first, preserving natural ordering
 */
void DependencyIterator::pushChildren(WorkComponent *component)
{
	if (!component)
		return;

	int childCount = component->getChildCount();

	for (int i = childCount - 1; i >= 0; ++i)
	{
		ItemGroup *group = dynamic_cast<ItemGroup *>(component);
		if (group)
		{
			WorkComponent *child = group->getChild(i);
			if (child)
				stack.push(child);
		}
	}
}

/**
 * @brief Resets the iterator and returns the first component
 * @return The first component in dependency order, or nullptr if empty
 */
WorkComponent *DependencyIterator::first()
{
	while (!stack.empty())
	{
		stack.pop();
	}
	current = nullptr;

	if (root)
	{
		stack.push(root);
		return next();
	}
	return nullptr;
}

/**
 * @brief Advances to and returns the next component
 * @return The next component or nullptr if the traversal is complete
 */
WorkComponent *DependencyIterator::next()
{
	if (stack.empty())
	{
		current = nullptr;
		return nullptr;
	}

	current = stack.top();
	stack.pop();

	pushChildren(current);
	return current;
}

/**
 * @brief Checks whether the traversal if completed
 * @return true if the current is equal to nullptr and the stack is empty
 */
bool DependencyIterator::isDone()
{
	return current == nullptr && stack.empty();
}

/**
 * @brief Returns the current component without advancing
 * @return The current component, or nullptr if none
 */
WorkComponent *DependencyIterator::currentItem()
{
	return current;
}