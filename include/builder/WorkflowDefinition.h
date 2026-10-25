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
		WorkflowInstance createInstance();
		WorkComponent *getRootTemplate();

		WorkflowInstance *createInstance();
		WorkComponent *getRootTemplate();
		const std::string &getId() const;
		const std::string &getName() const;
};

#endif
