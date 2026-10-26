#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H#include "ItemType.h"
#include "ItemStatus.h"
#include "../chain/Worker.h"

class FilterCriteria
{
private:	
	ItemStatus status;
	Worker *assignedTo;

public:
	FilterCriteria(ItemStatus s = ItemStatus::ANY, Worker *p = nullptr) : status(s), assignedTo(p)
	{
	}
	
	ItemStatus getStatus() const
	{
		return status;
	}
	Worker *getAssignedTo() const
	{
		return assignedTo;
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
