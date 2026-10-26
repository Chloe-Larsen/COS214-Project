#include "../../include/state/Completed.h"
#include "../../include/composite/Item.h"

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
}

void Completed::unblock(Item *item)
{
}

std::string Completed::getName() const
{
	return "COMPLETED";
}