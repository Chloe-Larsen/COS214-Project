#include "../../include/composite/Item.h"
#include "../../include/chain/Worker.h"
#include "../../include/state/State.h"
#include "../../include/state/Available.h"
#include "../../include/state/Approved.h"
#include "../../include/state/Blocked.h"
#include "../../include/state/Cancelled.h"
#include "../../include/state/Completed.h"
#include "../../include/state/InProgress.h"

#include <cstdio>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <ctime>

Item::Item(std::string name, std::string deadline, int priority, Worker *assignedTo) : WorkComponent(name), deadline(0), priority(priority), assignedTo(NULL), currentState(new Available()), dependencies(0), rejected(false), escalated(false)
{
    std::tm time = {};
    std::istringstream input(deadline);

    input >> std::get_time(&time, "%Y-%m-%d");

    if (!input.fail())
    {
        this->deadline = std::mktime(&time);
    }
    
	this->assignedTo = assignedTo;
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
	delete currentState;
}

void Item::progress()
{
	if(currentState != NULL)
    {
        currentState->progress(this);
    }
}

void Item::cancel()
{
	if(currentState != NULL)
    {
        currentState->cancel(this);
    }
}

void Item::reject(std::string reason)
{
	rejected = true;
	rejectionReason = reason;
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

	if (assignedTo == NULL)
	{
		reject("No reviewer available");
		return;
	}

	if (assignedTo->review(this, getPriority()))
	{
		setState(new Approved());
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
	
	if(currentState != NULL)
    {
        currentState->setEscalated();
    }
}

Worker *Item::getWorker()
{
	return assignedTo;
}

void Item::assignWorker(Worker *person)
{
	assignedTo = person;
}

State *Item::getState()
{
	return currentState;
}

void Item::setState(State *state)
{
	if(currentState != state)
    {
        delete currentState;
        currentState = state;
    }
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
	if(currentState == NULL)
    {
        return "Unknown";
    }

	return currentState->getName();
}