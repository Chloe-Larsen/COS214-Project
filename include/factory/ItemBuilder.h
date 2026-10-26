#ifndef ITEMBUILDER_H
#define ITEMBUILDER_H

#include "../composite/Item.h"
#include "ComponentBuilder.h"

class ItemBuilder : ComponentBuilder
{
private:
	Item *item;

protected:
	WorkComponent *createProduct();

public:
	ItemBuilder();
	~ItemBuilder();
	ItemBuilder *addDependency(Item *item);
	ItemBuilder *addWorker(Worker *worker);
	ItemBuilder *setDeadline(std::time_t deadline);
	ItemBuilder *setPriority(int priority);
	ItemBuilder *addNotifier();
	ItemBuilder *addAuditLogger();
};

#endif
