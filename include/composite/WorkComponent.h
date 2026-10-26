#ifndef WORKCOMPONENT_H
#define WORKCOMPONENT_H

#include "../observer/EventSource.h"
#include "../iterator/Iterator.h"
#include <string>

class Iterator;

class WorkComponent : public EventSource
{
protected:
	std::string id;
	std::string name;
	std::string status;

public:
	WorkComponent(std::string id, std::string name, std::string status);
	WorkComponent(const WorkComponent &other);
	virtual ~WorkComponent();
	virtual void progress() = 0;
	virtual void cancel() = 0;
	virtual void requestReview() = 0;
	virtual Iterator *createIterator(std::string type) = 0;
	virtual int getChildCount() = 0;
	virtual WorkComponent *clone() const = 0;

	const std::string &getId() const;
	const std::string &getName() const;
	const std::string &getStatus() const;
	void setStatus(const std::string &s);
};

#endif