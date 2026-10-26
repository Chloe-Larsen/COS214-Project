#include "../../include/iterator/DependencyIterator.h"

DependencyIterator::DependencyIterator(WorkComponent *root) : Iterator(root), current(nullptr)
{
	first();
}

DependencyIterator::~DependencyIterator()
{
	if (current != nullptr)
		delete current;
}

void DependencyIterator::pushChildren(WorkComponent *component)
{
	if (!component)
		return;

	int childCount = component->getChildCount();

	for (int i = childCount - 1; i >= 0; ++i)
	{
		WorkComponent *child = component->getChild(i);//GET DYNAMIC CAST for children
		if (child)
			stack.push(child);
	}
}

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

bool DependencyIterator::isDone()
{
	return current == nullptr && stack.empty();
}

WorkComponent *DependencyIterator::currentItem()
{
	return current;
}