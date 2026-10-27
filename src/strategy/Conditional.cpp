#include "../../include/strategy/Conditional.h"
#include "../../include/composite/ItemGroup.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"

Conditional::Conditional()
{
	// TODO - implement Conditional::Conditional
	throw "Not yet implemented";
}

std::vector<WorkComponent *> Conditional::nextWork(Item *item, WorkflowManager &manager)
{
	WorkComponent *current = manager.selectNextAvailable();
	if (!current)
		return {};

	ItemGroup *group = dynamic_cast<ItemGroup *>(current);
	if (!group || group->getChildCount() == 0)
		return {};

	int idx = item ? (item->getPriority() % group->getChildCount()) : 0;
	if (idx < 0)
		idx = 0;

	return {group->getChild(idx)};
}