#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "ItemType.h"
#include "ItemStatus.h"
#include "../chain/Worker.h"

class FilterCriteria
{
private:
	ItemType type;
	ItemStatus status;
	Worker *assignedTo;

public:
	FilterCriteria(ItemType t = ItemType::ALL, ItemStatus s = ItemStatus::ANY, Worker *p = nullptr) : type(t), status(s), assignedTo(p)
	{
	}

	ItemType getType() const
	{
		return type;
	}
	
	ItemStatus getStatus() const
	{
		return status;
	}
	Worker *getAssignedTo() const
	{
		return assignedTo;
	}

	void setType(ItemType t)
	{
		type = t;
	}

	void setStatus(ItemStatus s)
	{
		status = s;
	}
	void setAssignedTo(Worker *w)
	{
		assignedTo = w;
	}
};

#endif
