#include "../../include/chain/Worker.h"
#include "../../include/composite/Item.h"

Worker::Worker(std::string id, std::string name, std::string role, Worker *supervisor) : id(id), name(name), role(role), supervisor(supervisor), workload(0)
{
	if(supervisor)
	{
		supervisor->addSubordinate(this);
	}
}

bool Worker::review(Item *item, int depth)
{
	if(item == NULL || depth < 0 || depth > MAX_CHAIN_DEPTH)
	{
		return false;
	}

	// 1. This handler decides. A rejection terminates the chain immediately.
	bool approve = policy ? policy(item) : true;

	if (!approve)
	{
		item->reject(role + " " + name + " rejected the request");
		return false;
	}

	// 2. Is this approval level enough for the item's importance?
	int levelsRequired = item->getPriority() < 1 ? 1 : item->getPriority();
	
	if(depth + 1 >= levelsRequired)
	{
		return true;
	}

	// 3. Not enough sign-offs yet: forward to superior, or escalate if none.
	if(supervisor == NULL)
	{
		handleEscalation(item);
		return false;
	}

	return supervisor->review(item, depth + 1);
}

void Worker::handleEscalation(Item *item)
{
	if(item == NULL)
	{
		return;
	}

	item->escalate();                 // item is now pending manual resolution
	escalatedQueue.push_back(item);   // e.g. HR picks it up from here
}

int Worker::getWorkload()
{
	return workload;
}

void Worker::addWorkload(int delta)
{
	workload += delta;
	if (workload < 0)
	{
		workload = 0;
	}
}

void Worker::setSupervisor(Worker *s)
{
	supervisor = s;
}

void Worker::addSubordinate(Worker *w)
{
	subordinates.push_back(w);
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

const std::string &Worker::getRole() const
{ 
	return role;
}

const std::vector<Item *> &Worker::getEscalatedQueue() const
{ 
	return escalatedQueue;
}