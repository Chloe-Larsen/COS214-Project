#include "../../include/state/Available.h"
#include "../../include/state/Cancelled.h"
#include "../../include/state/Blocked.h"
#include "../../include/composite/Item.h"
#include "../../include/state/InProgress.h"

Available::Available() : isEscalated(false)
{
}

void Available::progress(Item *item)
{
	InProgress *next = new InProgress();
	if (isEscalated)
	{
		next->setEscalated();
	}
	item->setState(next);
}

void Available::cancel(Item *item)
{
	item->setState(new Cancelled());
}

void Available::block(Item *item)
{
	Blocked *next = new Blocked();
	if (isEscalated)
	{
		next->setEscalated();
	}
	item->setState(next);
}

void Available::unblock(Item *item)
{
}

bool Available::getEscalated()
{
	return isEscalated;
}

void Available::setEscalated()
{
	isEscalated = true;
}

std::string Available::getName() const
{
	return "AVAILABLE";
}