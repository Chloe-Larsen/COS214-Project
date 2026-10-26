#include "../../include/iterator/PriorityIterator.h"
#include "../../include/composite/ItemGroup.h"

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

PriorityIterator::PriorityIterator(WorkComponent *root) : Iterator(root), current(nullptr)
{
	first();
}

PriorityIterator::~PriorityIterator()
{
	if (current)
		delete current;
}

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

WorkComponent *PriorityIterator::first()
{
	while(!queue.empty())
	{
		queue.pop();
	}
	current=nullptr;

	if(root)
	{
		collectAll(root);
		return next();
	}
	return nullptr;
}

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

bool PriorityIterator::isDone()
{
	return current == nullptr && queue.empty();
}

WorkComponent *PriorityIterator::currentItem()
{
	return current;
}