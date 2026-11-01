#ifndef COMPONENTBUILDER_H
#define COMPONENTBUILDER_H

#include "../composite/WorkComponent.h"

/**
 * @class ComponentBuilder
 * @brief Abstract base class for the Builder pattern over WorkComponent trees.
 *
 * Defines the common interface for builders that construct concrete WorkComponent products (e.g., Item, ItemGroup).
 * Subclasses implement createProduct() to instantiate the specific product type, and provide fluent configuration methods that return the builder for chaining
 *
 * @note This is a "fluent builder": configuration methods in subclasses return the concrete builder type so calls can be chained.
 */

class ComponentBuilder
{
protected:
	/**
	 * @brief Creates a new, unconfigured product instance
	 * @return Pointer to the newly allocated product (ownership passed to caller)
	 *
	 * Implemented by concrete builders to instantiate the concrete product type
	 */
	virtual WorkComponent *createProduct() = 0;

public:
	/** @brief Constructs a ComponentBuilder */
	ComponentBuilder();

	/** @brief Virtual Deconstructor */
	virtual ~ComponentBuilder();

	/**
	 * @brief Sets the product's name
	 * @param name Name to assign
	 * @return This builder, for chaining
	 */
	virtual ComponentBuilder *setName(std::string name) = 0;

	/**
	 * @brief Returns the fully configured product
	 * @return Pointer to the built WorkComponent
	 */
	virtual WorkComponent *getProduct() = 0;
};

#endif
