#include "../../include/state/Approved.h"
#include "../../include/state/Completed.h"
#include "../../include/state/Cancelled.h"
#include "../../include/state/Blocked.h"
#include "../../include/composite/Item.h"

Approved::Approved()
{
}

void Approved::progress(Item *item)
{
	item->setState(new Completed());
}

void Approved::cancel(Item *item)
{
	item->setState(new Cancelled());
}

void Approved::block(Item *item)
{

}

void Approved::unblock(Item *item)
{
}

std::string Approved::getName() const
{
	return "APPROVED";
}
