#include "../../include/factory/ItemGroupBuilder.h"

ItemGroupBuilder::ItemGroupBuilder()
{
	itemGroup = createProduct();
}

ItemGroupBuilder::~ItemGroupBuilder()
{
	if(itemGroup)
		delete itemGroup;
}

ItemGroup *ItemGroupBuilder::createProduct()
{
	return new ItemGroup();
}

ItemGroupBuilder *ItemGroupBuilder::addChild(WorkComponent *child)
{
	itemGroup->add(child);
	return this;
}

ItemGroupBuilder *ItemGroupBuilder::setName(std::string name)
{
	itemGroup->setName(name);
	return this;
}

WorkComponent *ItemGroupBuilder::getProduct()
{
	return itemGroup;
}