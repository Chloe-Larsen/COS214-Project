#ifndef REJECT_H
#define REJECT_H

#include "WorkflowCommand.h"

class Reject : public WorkflowCommand {
private:
	std::string reason;
public:
	Reject();
	void execute() override;
	std::string getName() override;
};

#endif
