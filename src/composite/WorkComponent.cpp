#include "../../include/composite/WorkComponent.h"

WorkComponent::WorkComponent(std::string id, std::string name, std::string status) : EventSource(), id(id), name(name), status(status) {}

WorkComponent::WorkComponent(const WorkComponent &other) : EventSource(other), id(other.id), name(other.name), status(other.status) {}

WorkComponent::~WorkComponent() {}

const std::string &WorkComponent::getId() const
{
	return id;
}

const std::string &WorkComponent::getName() const
{
	return name;
}

const std::string &WorkComponent::getStatus() const
{
	return status;
}

void WorkComponent::setStatus(const std::string &s)
{
	status = s;
}
