#include "../../include/strategy/WorkloadBased.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"
#include <stdexcept>

WorkloadBased::WorkloadBased() {}

Worker *WorkloadBased::assign(Item *item, WorkflowManager *manager)
{
	if (!item)
		throw std::runtime_error("WorkloadBased: null item");

	Worker *w = item->getWorker();
	if (!w)
		throw std::runtime_error("WorkloadBased: item has no worker to evaluate");

	return w;
}