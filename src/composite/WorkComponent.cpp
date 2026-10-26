#include "../../include/composite/WorkComponent.h"

WorkComponent::WorkComponent(std::string id, std::string name, std::string status) : EventSource(), name(name) {}

WorkComponent::WorkComponent(const WorkComponent &other) : EventSource(other), name(other.name) {}

WorkComponent::~WorkComponent() {}
const std::string &WorkComponent::getName() const
{
	return name;
}
