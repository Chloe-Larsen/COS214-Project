/**
 * @file WorkComponent.cpp
 * @brief Component naming and shared event-source construction.
 * @ingroup tividy_composite
 */
#include "../../include/composite/WorkComponent.h"

WorkComponent::WorkComponent(std::string name) : EventSource(), name(name)
{
}

/**
 * @brief Copy constructor for the WorkComponent base class.
 * @param other The WorkComponent object being copied.
 * @details Copies the component name and EventSource data when creating
 *          a cloned component.
 */
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
