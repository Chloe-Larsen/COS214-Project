#include "../../include/state/Completed.h"
#include "../../include/composite/Item.h"
#include "../../include/state/Blocked.h"

Completed::Completed()
{
}

void Completed::progress(Item *item)
{
}

void Completed::cancel(Item *item)
{
}

void Completed::block(Item *item)
{
	item->setState(new Blocked());
}

void Completed::unblock(Item *item)
{
}

std::string Completed::getName() const
{
	return "COMPLETED";
}