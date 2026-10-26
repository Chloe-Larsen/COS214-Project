#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "Observer.h"

class Dependency : Observer
{
private:
	Item *dependentItem;

public:
	Dependency(Item *item);
	
	void update(Event *event) override;
	Dependency *clone() override;
};

#endif
