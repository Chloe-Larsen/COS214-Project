#ifndef REJECT_H
#define REJECT_H

#include "WorkflowCommand.h"

class Reject : public WorkflowCommand {
private:
	std::string reason;
public:
	Reject(Item* item);
	void execute() override;
	std::string getName() override;
};

#endif
