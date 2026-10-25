#include "../../include/composite/Item.h"

Item::Item(std::string id, std::string name, std::string status, std::string deadline, int priority, Worker *assignedTo, AssignmentStrategy *assignmentStrategy) : WorkComponent(id, name, status)
{
	// TODO - implement Item::Item
	throw "Not yet implemented";
}

Item::Item(Item &other) : WorkComponent(other)
{
	// TODO - implement Item::Item
	throw "Not yet implemented";
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

void Item::block()
{
	// TODO - implement Item::block
	throw "Not yet implemented";
}

void Item::unblock()
{
	// TODO - implement Item::unblock
	throw "Not yet implemented";
}

void Item::reject(std::string reason)
{
	// TODO - implement Item::reject
	throw "Not yet implemented";
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
	// TODO - implement Item::requestReview
	throw "Not yet implemented";
}

bool Item::isEscalated()
{
	// TODO - implement Item::isEscalated
	throw "Not yet implemented";
}

void Item::escalate()
{
	// TODO - implement Item::escalate
	throw "Not yet implemented";
}

Worker *Item::getWorker()
{
	// TODO - implement Item::getWorker
	throw "Not yet implemented";
}

void Item::assignWorker(Worker *person)
{
	// TODO - implement Item::assignWorker
	throw "Not yet implemented";
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

Item *Item::clone()
{
	// TODO - implement Item::clone
	throw "Not yet implemented";
}

Iterator *createIterator(std::string type)
{
}

int getChildCount()
{
}