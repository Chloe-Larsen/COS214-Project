#include "../../include/factory/ItemGroupBuilder.h"

/** @brief Constructs an ItemGroupBuilder and creates the underlying ItemGroup*/
ItemGroupBuilder::ItemGroupBuilder()
{
	itemGroup = createProduct();
}

/** @brief Destructor*/
ItemGroupBuilder::~ItemGroupBuilder()
{
	if (itemGroup)
		delete itemGroup;
}

/**
 * @brief Allocates a new ItemGroup
 * @return Newly allocated ItemGroup
 */
ItemGroup *ItemGroupBuilder::createProduct()
{
	return new ItemGroup();
}

/**
 * @brief Adds a child component to the group
 * @param child Child component to add
 * @return This builder, for chaining
 */
ItemGroupBuilder *ItemGroupBuilder::addChild(WorkComponent *child)
{
	itemGroup->add(child);
	return this;
}

/**
 * @brief Sets the group's name
 * @param name Name to assign
 * @return This builder, for chaining
 */
ItemGroupBuilder *ItemGroupBuilder::setName(std::string name)
{
	itemGroup->setName(name);
	return this;
}

/**
 * @brief Returns the built Item
 * @return Pointer to the configured Item
 */
WorkComponent *ItemGroupBuilder::getProduct()
{
	return itemGroup;
}