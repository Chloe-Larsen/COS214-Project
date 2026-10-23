#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "Observer.h"


class Dependency : Observer {
private:
	WorkComponent* dependentItem;
public:
	Dependency(WorkComponent* item);
	void update(Event* event) override;
};

#endif
