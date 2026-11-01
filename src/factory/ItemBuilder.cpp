#include "../../include/factory/ItemBuilder.h"
#include "../../include/observer/Dependency.h"
#include "../../include/observer/Notification.h"
#include "../../include/observer/AuditLogger.h"
#include "../../include/adapter/EmailServiceAdapter.h"

/** @brief Constructs an ItemBuilder and creates the underlying Item */
ItemBuilder::ItemBuilder() : ComponentBuilder()
{
	item = createProduct();
}

/** @brief Destructor */
ItemBuilder::~ItemBuilder()
{
	if (item)
		delete item;
}

/**
 * @brief Allocates a new Item
 * @return Newly allocated Item
 */
Item *ItemBuilder::createProduct()
{
	return new Item();
}

/**
 * @brief Attaches a Dependency observer so this item depends on another
 * @param item The item this one depends on
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::addDependency(Item *item)
{
	Dependency *dependency = new Dependency(item);
	this->item->attach(dependency);
	return this;
}

/**
 * @brief Assigns a Worker to the item
 * @param worker Worker to assign
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::addWorker(Worker *worker)
{
	item->assignWorker(worker);
	return this;
}

/**
 * @brief Sets the item's deadline
 * @param deadline Deadline as a std::time_t value
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::setDeadline(std::time_t deadline)
{
	item->setDeadline(deadline);
	return this;
}

/**
 * @brief Sets the item's priority
 * @param priority Priority value (higher = more important, depending on comparator)
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::setPriority(int priority)
{
	item->setPriority(priority);
	return this;
}

/**
 * @brief Attaches an email notifier observer to the item
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::addNotifier()
{
	EmailServiceAdapter *email = new EmailServiceAdapter();
	Notification *notifier = new Notification(email);
	item->attach(notifier);
	return this;
}

/**
 * @brief Attaches an AuditLogger observer to the item
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::addAuditLogger()
{
	AuditLogger *auditLogger = new AuditLogger();
	item->attach(auditLogger);
	return this;
}

/**
 * @brief Sets the item's name
 * @param name Name to assign
 * @return This builder, for chaining
 */
ItemBuilder *ItemBuilder::setName(std::string name)
{
	item->setName(name);
	return this;
}

/**
 * @brief Returns the built Item
 * @return Pointer to the configured Item
 */
WorkComponent *ItemBuilder::getProduct()
{
	return item;
}