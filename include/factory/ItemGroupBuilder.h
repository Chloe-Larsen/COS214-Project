#ifndef ITEMGROUPBUILDER_H
#define ITEMGROUPBUILDER_H

#include "../composite/ItemGroup.h"
#include "ComponentBuilder.h"

class ItemGroupBuilder : ComponentBuilder
{

private:
	ItemGroup *itemGroup;

protected:
	ItemGroup *createProduct() override;

public:
	ItemGroupBuilder();
	~ItemGroupBuilder();
	ItemGroupBuilder *addChild(WorkComponent *child);
	ItemGroupBuilder *setName(std::string name) override;
	WorkComponent *getProduct() override;
};

#endif
