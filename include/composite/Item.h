#ifndef ITEM_H
#define ITEM_H

#include "WorkComponent.h"
#include <ctime>
#include "../chain/Worker.h"
#include "../state/State.h"
#include "../strategy/AssignmentStrategy.h"

class Item : public WorkComponent {

private:
	std::time_t deadline;
	int priority;
	Worker* assignedTo;
	State* currentState;
	AssignmentStrategy* assignmentStrategy;
	int dependencies;

public:
	Item(std::string id, std::string name, std::string status, std::string deadline, int priority, Worker* assignedTo, AssignmentStrategy* assignmentStrategy);
	Item(Item& other);
	~Item();
	void progress();
	void cancel();
	void reject(std::string reason);
	void incrementDependencies();
	void decrementDependencies();
	void requestReview();
	bool isEscalated();
	void escalate();
	Worker* getWorker();
	void assignWorker(Worker* person);
	State* getState();
	void setState(State* state);
	int getPriority();
	void setPriority(int priority);
	Item* clone();	
	Iterator* createIterator(std::string type) override;
	int getChildCount() override;	
};

#endif
