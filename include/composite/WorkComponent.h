#ifndef WORKCOMPONENT_H
#define WORKCOMPONENT_H

#include "../observer/EventSource.h"
#include "../iterator/Iterator.h"

class WorkComponent : public EventSource {
protected:
	std::string id;
	std::string name;
	std::string status;

public:
	WorkComponent(std::string id, std::string name, std::string status);
	WorkComponent(WorkComponent& other);
	virtual ~WorkComponent();
	virtual void progress() = 0;
	virtual void cancel() = 0;
	virtual void requestReview() = 0;
	virtual Iterator* createIterator(std::string type) = 0;
	virtual int getChildCount() = 0;
	virtual WorkComponent* clone() = 0;
	virtual WorkComponent* getChild(int i) = 0;
};

#endif
