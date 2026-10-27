#include "../../include/strategy/RuleBased.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"
#include <stdexcept>

RuleBased::RuleBased() {}

Worker *RuleBased::assign(Item *item, WorkflowManager *manager)
{
	if (!item)
		throw std::runtime_error("RuleBased: null item");

	Worker *w = item->getWorker();
	if (!w)
		throw std::runtime_error("RuleBased: item has no worker");

	return w;
}