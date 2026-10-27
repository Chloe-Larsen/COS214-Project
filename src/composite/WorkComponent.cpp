#include "../../include/composite/WorkComponent.h"

WorkComponent::WorkComponent(std::string name) : EventSource(), name(name)
{
}

WorkComponent::WorkComponent(const WorkComponent &other) : EventSource(other), name(other.name)
{
}

WorkComponent::~WorkComponent()
{
}

const std::string &WorkComponent::getName() const
{
	return name;
}

void WorkComponent::setName(std::string name)
{
	this->name = name;
}
