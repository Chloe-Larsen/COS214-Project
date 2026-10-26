#include "../../include/strategy/Parallel.h"
#include "../../include/composite/ItemGroup.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"

Parallel::Parallel()
{
}

std::vector<WorkComponent *> Parallel::nextWork(Item *item, WorkflowManager &manager)
{
	WorkComponent *current = manager.selectNextAvailable();
	if (!current)
		return {};

	ItemGroup *group = dynamic_cast<ItemGroup *>(current);
	if (!group)
		return {};

	std::vector<WorkComponent *> result;
	int n = group->getChildCount();
	for (int i = 0; i < n; ++i)
		result.push_back(group->getChild(i));
	return result;
}