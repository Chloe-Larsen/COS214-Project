#include "../../include/factory/ItemBuilder.h"
#include "../../include/observer/Dependency.h"
#include "../../include/observer/Notification.h"
#include "../../include/observer/AuditLogger.h"
#include "../../include/adapter/EmailServiceAdapter.h"

ItemBuilder::ItemBuilder() : ComponentBuilder()
{
	item = createProduct();
}

ItemBuilder::~ItemBuilder()
{
	if (item)
		delete item;
}

Item *ItemBuilder::createProduct()
{
	return new Item();
}

ItemBuilder *ItemBuilder::addDependency(Item *item)
{
	Dependency *dependency = new Dependency(item);
	this->item->attach(dependency);
	return this;
}

ItemBuilder *ItemBuilder::addWorker(Worker *worker)
{
	item->assignWorker(worker);
	return this;
}

ItemBuilder *ItemBuilder::setDeadline(std::time_t deadline)
{
	item->setDeadline(deadline);
	return this;
}

ItemBuilder *ItemBuilder::setPriority(int priority)
{
	item->setPriority(priority);
	return this;
}

ItemBuilder *ItemBuilder::addNotifier()
{
	EmailServiceAdapter *email = new EmailServiceAdapter();
	Notification *notifier = new Notification(email);
	item->attach(notifier);
	return this;
}

ItemBuilder *ItemBuilder::addAuditLogger()
{
	AuditLogger *auditLogger = new AuditLogger();
	item->attach(auditLogger);
	return this;
}

ItemBuilder *ItemBuilder::setName(std::string name)
{
	item->setName(name);
	return this;
}

WorkComponent *ItemBuilder::getProduct()
{
	return item;
}