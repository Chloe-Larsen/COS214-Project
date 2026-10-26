#ifndef ITEMGROUPBUILDER_H
#define ITEMGROUPBUILDER_H

#include "../composite/ItemGroup.h"
#include "ComponentBuilder.h"

class ItemGroupBuilder : ComponentBuilder
{

private:
	ItemGroup *itemGroup;

protected:
	WorkComponent *createProduct();

public:
	ItemGroupBuilder();
	~ItemGroupBuilder();
	ItemGroupBuilder *addChild(WorkComponent *child);
};

#endif
