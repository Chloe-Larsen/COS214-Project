#ifndef ROLEBASED_H
#define ROLEBASED_H

#include "AssignmentStrategy.h"

class RoleBased : AssignmentStrategy {
private:
	std::string role;
public:	
	RoleBased();
	Worker assign(Item item, WorkflowInstance instance) override;
};

#endif
