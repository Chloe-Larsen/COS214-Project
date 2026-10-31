/**
 * @file Worker.cpp
 * @brief Implements worker approval, escalation, and workload calculation.
 */

#include "../../include/chain/Worker.h"
#include "../../include/composite/Item.h"
#include "../../include/composite/ItemGroup.h"
#include "../../include/iterator/DependencyIterator.h"

#include <random>

/**
 * @brief Constructs a worker and connects them to their supervisor.
 *
 * @param id Unique identifier of the worker.
 * @param name Name of the worker.
 * @param role Role held by the worker.
 * @param supervisor Direct supervisor of the worker.
 *
 * The worker is automatically registered as a subordinate of the
 * supplied supervisor.
 */
Worker::Worker(std::string id, std::string name, std::string role, Worker *supervisor) : id(id), name(name), role(role), supervisor(supervisor), workflowRoot(NULL)
{
	if (supervisor)
	{
		supervisor->addSubordinate(this);
	}
}

/**
 * @brief Processes an approval request using the Chain of Responsibility pattern.
 *
 * @param item Item that requires approval.
 * @param depth Number of approval levels remaining.
 * @return true if the request receives all required approvals;
 *         false if rejected, escalated, or invalid.
 *
 * Each worker makes an approval decision. A rejection immediately
 * terminates the chain. If further approval is required, the request
 * is passed to the supervisor. If no supervisor exists, the request
 * is escalated instead.
 */
bool Worker::review(Item *item, int depth)
{
	if (item == NULL || depth < 1 || depth > MAX_CHAIN_DEPTH)
	{
		return false;
	}

	// Decides whether this worker approves or rejects
	bool approve;

	if(policy)
	{
		approve = policy(item);
	}
	else
	{
		static std::random_device rd;
		static std::mt19937 generator(rd());
		static std::uniform_int_distribution<int> distribution(1, 100);

		approve = distribution(generator) <= 80; // 80% chance of approval
	}

	// if this worker rejects, stop chain immediately
	if(!approve)
	{
		item->reject(role + " " + name + " rejected the request");
		return false;
	}

	// this worker approved, if no more levels are required
	if (depth == 1)
	{
		return true;
	}

	// more approvals required, forward to supervisor
	if (supervisor != NULL)
	{
		return supervisor->review(item, depth - 1);
	}
	
	handleEscalation(item);
	return false; // escalated, so not approved yet
}

/**
 * @brief Handles an approval request that cannot continue through the hierarchy.
 *
 * @param item Item that requires escalation.
 *
 * The item is marked as escalated, assigned to this worker, and added
 * to the escalation queue if it is not already present.
 */
void Worker::handleEscalation(Item *item)
{
    if (item == nullptr){
        return;
    }

    if (!item->isEscalated()){
        item->escalate();
    }

    for (Item *queued : escalatedQueue){
        if (queued == item){
            return;
        }
    }

	item->assignWorker(this);
	item->escalate();

	escalatedQueue.push_back(item);
}

/**
 * @brief Sets the root workflow used to calculate this worker's workload.
 * @param root Root component of the workflow tree.
 */
void Worker::setWorkflowRoot(WorkComponent *root)
{
	workflowRoot = root;
}

/**
 * @brief Calculates workload by traversing the workflow tree.
 * @return Number of items assigned to this worker.
 *
 * DependencyIterator visits the components in the workflow hierarchy.
 * Each component is checked to determine whether it is an Item assigned
 * to this worker. Groups are traversed but are not included in the count.
 */
int Worker::getWorkload()
{
	if(workflowRoot == NULL)
	{
		return 0;
	}

	int count = 0;

	DependencyIterator iterator(workflowRoot);

	for(WorkComponent *component = iterator.first(); component != NULL; component = iterator.next())
	{
		Item *item = dynamic_cast<Item *>(component);

		if(item && item->getWorker() == this)
		{
			count++;
		}
	}

	return count;
}

/** @brief Updates the worker's direct supervisor. */
void Worker::setSupervisor(Worker *s)
{
	supervisor = s;
}

/** @brief Adds a worker to this worker's subordinate list. */
void Worker::addSubordinate(Worker *w)
{
	if(w != NULL)
	{
		subordinates.push_back(w);
	}
}

/** @brief Sets the approval decision policy for this worker. */
void Worker::setDecisionPolicy(DecisionPolicy p)
{
	policy = p;
}

/** @brief Returns the worker's direct supervisor. */
Worker *Worker::getSupervisor() const
{
	return supervisor;
}

/** @brief Returns the worker's unique identifier. */
const std::string &Worker::getId() const
{
	return id;
}

/** @brief Returns the worker's name. */
const std::string &Worker::getName() const
{
	return name;
}

/** @brief Returns the worker's email address. */
const std::string &Worker::getEmail() const
{
	return email;
}

/** @brief Returns the worker's role. */
const std::string &Worker::getRole() const
{
	return role;
}

/** @brief Returns the worker's escalation queue. */
const std::vector<Item *> &Worker::getEscalatedQueue() const
{
	return escalatedQueue;
}