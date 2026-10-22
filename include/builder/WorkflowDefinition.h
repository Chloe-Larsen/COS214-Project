#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include "../WorkflowInstance.h"
#include "../composite/WorkComponent.h"
#include <string>

class WorkflowDefinition
{
private:
	std::string id;
	std::string name;
	WorkComponent *rootTemplate;

public:
	WorkflowInstance createInstance();
	WorkComponent *getRootTemplate();
};

#endif
