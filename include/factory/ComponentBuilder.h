#ifndef COMPONENTBUILDER_H
#define COMPONENTBUILDER_H

#include "../composite/WorkComponent.h"

class ComponentBuilder
{
protected:
	virtual WorkComponent *createProduct() = 0;

public:
	ComponentBuilder();
	virtual ~ComponentBuilder();
	virtual ComponentBuilder *setName(std::string name) = 0;
	virtual WorkComponent *getProduct() = 0;
};

#endif
