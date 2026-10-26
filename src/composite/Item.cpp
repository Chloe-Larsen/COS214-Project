#include "../../include/composite/Item.h"
#include "../../include/chain/Worker.h"
#include "../../include/state/State.h"
#include <cstdio>
#include <stdexcept>

Item::Item(std::string name, std::string deadline, int priority, Worker *assignedTo) : WorkComponent(name)
{
	// TODO - implement Item::Item
	throw "Not yet implemented";
}

// PROTOTYPE: copy configuration, reset runtime state so instances are isolated.
Item::Item(const Item &other) : WorkComponent(other), deadline(other.deadline), priority(other.priority), assignedTo(other.assignedTo), currentState(NULL), dependencies(other.dependencies), rejected(false), escalated(false)
{
}

Item *Item::clone() const
{
	return new Item(*this);
}

Item::~Item()
{
}

void Item::progress()
{
	// TODO - implement Item::progress
	throw "Not yet implemented";
}

void Item::cancel()
{
	// TODO - implement Item::cancel
	throw "Not yet implemented";
}

void Item::reject(std::string reason)
{
	rejected = true;
	rejectionReason = reason;
	status = "Rejected";
}

void Item::incrementDependencies()
{
	// TODO - implement Item::incrementDependencies
	throw "Not yet implemented";
}

void Item::decrementDependencies()
{
	// TODO - implement Item::decrementDependencies
	throw "Not yet implemented";
}

void Item::requestReview()
{
	rejected = false;
	escalated = false;
	rejectionReason.clear();

	if (assignedTo == NULL || assignedTo->getSupervisor() == NULL)
	{
		reject("No reviewer available for item " + id);
		return;
	}

	if (assignedTo->getSupervisor()->review(this, 0))
	{
		status = "Approved";
	}
}

bool Item::isEscalated() const
{
	return escalated;
}

bool Item::isRejected() const
{
	return rejected;
}

const std::string &Item::getRejectionReason() const
{
	return rejectionReason;
}

void Item::escalate()
{
	escalated = true;
	status = "Escalated";
}

Worker *Item::getWorker()
{
	return assignedTo;
}

void Item::assignWorker(Worker *person)
{
	if (assignedTo)
	{
		assignedTo->addWorkload(-1);
	}

	assignedTo = person;

	if (assignedTo)
	{
		assignedTo->addWorkload(1);
	}
}

State *Item::getState()
{
	return currentState;
}

void Item::setState(State *state)
{
	currentState = state;
}

int Item::getPriority()
{
	return this->priority;
}

void Item::setPriority(int priority)
{
	this->priority = priority;
}

Iterator *Item::createIterator(std::string type)
{
}

int Item::getChildCount()
{
	return 1;
}

std::string Item::getStatus() const
{
	return currentState->getName();
}

void Item::setDeadline(std::time_t deadline)
{
	this->deadline = deadline;
}