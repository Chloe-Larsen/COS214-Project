#ifndef ITEMGROUPBUILDER_H
#define ITEMGROUPBUILDER_H

#include "../composite/ItemGroup.h"
#include "ComponentBuilder.h"

/**
 * @class ItemGroupBuilder
 * @brief Concrete builder that constructs and configures an ItemGroup.
 *
 * Supports fluent configuration of an ItemGroup's name and children.
 * Children may be any WorkComponent, allowing nested groups and items.
 */

class ItemGroupBuilder : public ComponentBuilder
{

private:
	/** @brief The ItemGroup instance under construction */
	ItemGroup *itemGroup;

protected:
	/**
	 * @brief Allocates a new ItemGroup
	 * @return Newly allocated ItemGroup
	 */
	ItemGroup *createProduct() override;

public:
	/** @brief Constructs an ItemGroupBuilder and creates the underlying ItemGroup*/
	ItemGroupBuilder();

	/** @brief Destructor*/
	~ItemGroupBuilder();

	/**
	 * @brief Adds a child component to the group
	 * @param child Child component to add
	 * @return This builder, for chaining
	 */
	ItemGroupBuilder *addChild(WorkComponent *child);

	/**
	 * @brief Sets the group's name
	 * @param name Name to assign
	 * @return This builder, for chaining
	 */
	ItemGroupBuilder *setName(std::string name) override;

	/**
	 * @brief Returns the built Item
	 * @return Pointer to the configured Item
	 */
	WorkComponent *getProduct() override;
};

#endif
