#include "../../include/strategy/RoleBased.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"
#include "../../include/chain/Worker.h"

RoleBased::RoleBased(const std::string &role) : role(role) {}

Worker *RoleBased::assign(Item *item, WorkflowManager *manager)
{
	if (manager == nullptr || item == nullptr)
	{
		return nullptr;
	}

	if (role.empty())
	{
		return nullptr;
	}

	Worker *current = item->getWorker();
	if (current != nullptr && current->getRole() == role)
	{
		return current;
	}

	Worker *best = nullptr;
	int bestLoad = 0;

	for (Worker *worker : manager->getWorkers())
	{
		if (worker == nullptr)
		{
			continue;
		}
		if (worker->getRole() != role)
		{
			continue;
		}

		const int load = worker->getWorkload();
		if (best == nullptr || load < bestLoad)
		{
			best = worker;
			bestLoad = load;
		}
	}

	if (best != nullptr)
	{
		item->assignWorker(best);
	}
	return best;
}
