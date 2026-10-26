#include "../../include/state/Available.h"
#include "../../include/state/Cancelled.h"
#include "../../include/state/Blocked.h"
#include "../../include/composite/Item.h"
#include "../../include/state/InProgress.h"

Blocked::Blocked() : isEscalated(false)
{
}

void Blocked::progress(Item *item)
{

}

void Blocked::cancel(Item *item)
{
	item->setState(new Cancelled());
}

void Blocked::block(Item *item)
{
	
}

void Blocked::unblock(Item *item)
{
	Available* next = new Available();
	if (isEscalated)
	{
		next->setEscalated();
	}
	item->setState(next);
}

bool Blocked::getEscalated()
{
	return isEscalated;	
}

void Blocked::setEscalated()
{
	isEscalated = true;
}

std::string Blocked::getName() const
{
	return "BLOCKED";
}