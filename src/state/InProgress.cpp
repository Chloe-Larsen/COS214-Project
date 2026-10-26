#include "../../include/state/InProgress.h"
#include "../../include/state/Approved.h"
#include "../../include/state/Cancelled.h"
#include "../../include/state/Blocked.h"
#include "../../include/composite/Item.h"

InProgress::InProgress() : isEscalated(false)
{
}

void InProgress::progress(Item *item)
{
	item->setState(new Approved());
}

void InProgress::cancel(Item *item)
{
	item->setState(new Cancelled());
}

void InProgress::block(Item *item)
{
	Blocked *next = new Blocked();
	if (isEscalated)
	{
		next->setEscalated();
	}
	item->setState(next);
}

void InProgress::unblock(Item *item)
{
}

bool InProgress::getEscalated()
{
	return isEscalated;
}

void InProgress::setEscalated()
{
	isEscalated = true;
}

std::string InProgress::getName() const
{
	return "IN_PROGRESS";
}