#ifndef ROUTING_H
#define ROUTING_H

#include <vector>

#include "../composite/WorkComponent.h"

class Routing
{
public:
	Routing();
	virtual ~Routing();
	virtual std::vector<WorkComponent *> nextWork(Item item) = 0;
};

#endif
