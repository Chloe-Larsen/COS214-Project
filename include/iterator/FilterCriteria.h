#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "ItemType.h"
#include "ItemStatus.h"
#include "../chain/Worker.h"

class FilterCriteria
{
public:
	ItemType type;
	ItemStatus status;
	Worker *assignedTo;
};

#endif
