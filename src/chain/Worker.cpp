#include "../../include/chain/Worker.h"
#include "../../include/composite/Item.h"
#include "../../include/composite/ItemGroup.h"
#include "../../include/iterator/DependencyIterator.h"

#include <random>

Worker::Worker(std::string id, std::string name, std::string role, Worker *supervisor) : id(id), name(name), role(role), supervisor(supervisor), workflowRoot(NULL)
{
	if (supervisor)
	{
		supervisor->addSubordinate(this);
	}
}

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

void Worker::setWorkflowRoot(WorkComponent *root)
{
	workflowRoot = root;
}

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

void Worker::setSupervisor(Worker *s)
{
	supervisor = s;
}

void Worker::addSubordinate(Worker *w)
{
	if(w != NULL)
	{
		subordinates.push_back(w);
	}
}

void Worker::setDecisionPolicy(DecisionPolicy p)
{
	policy = p;
}

Worker *Worker::getSupervisor() const
{
	return supervisor;
}

const std::string &Worker::getId() const
{
	return id;
}

const std::string &Worker::getName() const
{
	return name;
}

const std::string &Worker::getEmail() const
{
	return email;
}

const std::string &Worker::getRole() const
{
	return role;
}

const std::vector<Item *> &Worker::getEscalatedQueue() const
{
	return escalatedQueue;
}