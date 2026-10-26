#include "../../include/strategy/RoleBased.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"
#include <stdexcept>

RoleBased::RoleBased()
{
}

Worker RoleBased::assign(Item *item, WorkflowManager &manager)
{
	if (!item)
		throw std::runtime_error("RoleBased: null item");

	Worker *w = item->getWorker();
	if (!w)
		throw std::runtime_error("RoleBased: item has no worker to evaluate");

	return *w;
}

//no way to select by role? question not statement