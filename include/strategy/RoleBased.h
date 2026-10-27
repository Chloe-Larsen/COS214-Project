#ifndef ROLEBASED_H
#define ROLEBASED_H

#include "AssignmentStrategy.h"

class RoleBased : public AssignmentStrategy
{
private:
	std::string role;

public:
	RoleBased();
	Worker *assign(Item *item, WorkflowManager *manager) override;
};

#endif
