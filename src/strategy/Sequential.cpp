#include "../../include/strategy/Sequential.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"

Sequential::Sequential()
{
}

std::vector<WorkComponent *> Sequential::nextWork(Item *item, WorkflowManager &manager)
{
	WorkComponent *current = manager.selectNextAvailable();
	if (!current)
		return {};

	ItemGroup *group = dynamic_cast<ItemGroup *>(current);
	if (!group || group->getChildCount() == 0)
		return {};

	return {group->getChild(0)};
}