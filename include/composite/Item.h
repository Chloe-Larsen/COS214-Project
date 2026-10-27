#ifndef ITEM_H
#define ITEM_H

#include "WorkComponent.h"
#include <ctime>
#include "../chain/Worker.h"
#include "../state/State.h"

class Worker;
class State;

class Item : public WorkComponent
{
private:
	std::time_t deadline;
	int priority;
	Worker *assignedTo;
	State *currentState;	
	int dependencies;
	bool rejected;
	std::string rejectionReason;
	Item &operator=(const Item &);

public:
	Item(std::string name, std::string deadline, int priority, Worker *assignedTo);
	Item(const Item &other);
	Item();
	~Item();
	void progress() override;
	void cancel() override;
	void reject(std::string reason);
	void incrementDependencies();
	void decrementDependencies();
	int getDependencies() const;
	void requestReview() override;
	bool isEscalated() const;
	bool isRejected() const;
	const std::string &getRejectionReason() const;
	void escalate();
	Worker *getWorker();
	void assignWorker(Worker *person);
	State *getState();
	void setState(State *state);
	int getPriority();
	void setPriority(int priority);
	std::time_t getDeadline() const;
	Item *clone() const override;
	Iterator *createIterator(std::string type) override;
	int getChildCount() override;
	std::string getStatus() const;
	void setDeadline(std::time_t deadline);
};

#endif
