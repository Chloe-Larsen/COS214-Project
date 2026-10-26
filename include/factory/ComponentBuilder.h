#ifndef COMPONENTBUILDER_H
#define COMPONENTBUILDER_H

#include "../composite/WorkComponent.h"

class ComponentBuilder
{
protected:
	WorkComponent *product;
	virtual WorkComponent *createProduct() = 0;

public:
	ComponentBuilder();
	virtual ~ComponentBuilder();
	ComponentBuilder *setName();
	WorkComponent *getProduct();
};

#endif
