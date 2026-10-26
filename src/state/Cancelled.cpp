#include "../../include/state/Cancelled.h"

Cancelled::Cancelled()
{
}

void Cancelled::progress(Item *item)
{
}

void Cancelled::cancel(Item *item)
{
}

void Cancelled::block(Item *item)
{
}

void Cancelled::unblock(Item *item)
{
}

std::string Cancelled::getName() const
{
	return "CANCELLED";
}