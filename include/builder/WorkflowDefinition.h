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

	WorkflowDefinition(const WorkflowDefinition &);
	WorkflowDefinition &operator=(const WorkflowDefinition &);

public:
	WorkflowDefinition(std::string id, std::string name);
	WorkflowInstance *createInstance();
	WorkComponent *getRootTemplate();
	void setRootTemplate(WorkComponent *root);
	const std::string &getId() const;
	const std::string &getName() const;
};

#endif
