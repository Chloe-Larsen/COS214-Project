#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <ctime>
#include <string>
#include "builder/WorkflowDefinition.h"
#include "strategy/Routing.h"

class WorkflowDefinition;
class WorkComponent;
class Routing;
class Iterator;

class WorkflowInstance
{
	private:
		std::string id;
		WorkflowDefinition *definition;
		WorkComponent *root;
		std::time_t createdAt;
		std::string status;
		Routing *routing;		
		WorkflowInstance &operator=(const WorkflowInstance &);

	public:
		WorkflowInstance();
		WorkflowInstance(const WorkflowInstance &);
		~WorkflowInstance();
		void buildFromDefinition(WorkflowDefinition *def);
		WorkComponent *getRoot() const;
		void evaluateReadiness();
		WorkComponent *selectNextAvailable();
		void progress();
		void cancel();
		Iterator* createIterator();
		void setRoot(WorkComponent *root);
		std::string getId();
		std::string getStatus();
};

#endif
