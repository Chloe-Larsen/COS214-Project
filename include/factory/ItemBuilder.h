#ifndef ITEMBUILDER_H
#define ITEMBUILDER_H

#include "../composite/Item.h"
#include "ComponentBuilder.h"

/**
 * @class ItemBuilder
 * @brief Concrete builder that constructs and configures an Item
 *
 * Supports fluent configuration of an Item's name, priority, deadline, assigned worker, and attached observers (dependencies, notifier and audit logger)
 *
 * @note The builder returns ItemBuilder* from chained calls, so users should store the result in an ItemBuilder*
 */
class ItemBuilder : public ComponentBuilder
{
private:
	/** @brief The Item instance under construction */
	Item *item;

protected:
	/**
	 * @brief Allocates a new Item
	 * @return Newly allocated Item
	 */
	Item *createProduct() override;

public:
	/** @brief Constructs an ItemBuilder and creates the underlying Item */
	ItemBuilder();

	/** @brief Destructor */
	~ItemBuilder();

	/**
	 * @brief Attaches a Dependency observer so this item depends on another
	 * @param item The item this one depends on
	 * @return This builder, for chaining
	 */
	ItemBuilder *addDependency(Item *item);

	/**
	 * @brief Assigns a Worker to the item
	 * @param worker Worker to assign
	 * @return This builder, for chaining
	 */
	ItemBuilder *addWorker(Worker *worker);

	/**
	 * @brief Sets the item's deadline
	 * @param deadline Deadline as a std::time_t value
	 * @return This builder, for chaining
	 */
	ItemBuilder *setDeadline(std::time_t deadline);

	/**
	 * @brief Sets the item's priority
	 * @param priority Priority value (higher = more important, depending on comparator)
	 * @return This builder, for chaining
	 */
	ItemBuilder *setPriority(int priority);

	/**
	 * @brief Attaches an email notifier observer to the item
	 * @return This builder, for chaining
	 */
	ItemBuilder *addNotifier();

	/**
	 * @brief Attaches an AuditLogger observer to the item
	 * @return This builder, for chaining
	 */
	ItemBuilder *addAuditLogger();

	/**
	 * @brief Sets the item's name
	 * @param name Name to assign
	 * @return This builder, for chaining
	 */
	ItemBuilder *setName(std::string name) override;

	/**
	 * @brief Returns the built Item
	 * @return Pointer to the configured Item
	 */
	WorkComponent *getProduct() override;
};

#endif
