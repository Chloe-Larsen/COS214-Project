#include "../../include/strategy/WorkloadBased.h"
#include "../../include/composite/Item.h"
#include "../../include/WorkflowManager.h"

WorkloadBased::WorkloadBased() {}

Worker *WorkloadBased::assign(Item *item, WorkflowManager *manager)
{
if (manager == nullptr || item == nullptr)
    {
        return nullptr;
    }

    Worker *best = nullptr;
    int bestLoad = 0;

    for (Worker *worker : manager->getWorkers())
    {
        if (worker == nullptr)
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

    return best;
}