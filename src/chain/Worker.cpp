#include "../../include/chain/Worker.h"

Worker::Worker(std::string id, std::string name, std::string email, std::string role, Worker *supervisor) : id(id), name(name), email(email), role(role), supervisor(supervisor) {}

bool Worker::review(Item *item, int depth)
{
	// TODO - implement Worker::review
	throw "Not yet implemented";
}

void Worker::handleEscalation(Item *item)
{
	// TODO - implement Worker::handleEscalation
	throw "Not yet implemented";
}

int Worker::getWorkload()
{
	// TODO - implement Worker::getWorkload
	throw "Not yet implemented";
}

std::string Worker::getId()
{
	return id;
}

std::string Worker::getName()
{
	return name;
}

std::string Worker::getEmail()
{
	return email;
}

std::string Worker::getRole()
{
	return role;
}

Worker *Worker::getSupervisor()
{
	return supervisor;
}