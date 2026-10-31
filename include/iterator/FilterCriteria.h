#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "../chain/Worker.h"
#include <vector>
#include <string>

static const std::string KNOWN_STATUSES[] = {
	"AVAILABLE",
	"IN_PROGRESS",
	"COMPLETED",
	"APPROVED",
	"BLOCKED",
	"CANCELLED"};

static const std::size_t KNOWN_STATUS_COUNT =
	sizeof(KNOWN_STATUSES) / sizeof(KNOWN_STATUSES[0]);

class FilterCriteria
{
private:
	Worker *assignedTo;
	bool selected[KNOWN_STATUS_COUNT];
	bool matchAnyStatus;

public:
	FilterCriteria(Worker *p = nullptr) : matchAnyStatus(true), assignedTo(p)
	{
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
			selected[i] = false;
	}

	bool isMatchAnyStatus() const
	{
		return matchAnyStatus;
	}

	bool setStatus(const std::string &name)
	{
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
		{
			if (KNOWN_STATUSES[i] == name)
			{
				for (std::size_t j = 0; j < KNOWN_STATUS_COUNT; ++j)
					selected[j] = false;
				selected[i] = true;
				matchAnyStatus = false;
				return true;
			}
		}
		return false;
	}
	
	void setStatus(std::size_t index)
	{
		if (index >= KNOWN_STATUS_COUNT)
			return;
		for (std::size_t j = 0; j < KNOWN_STATUS_COUNT; ++j)
			selected[j] = false;
		selected[index] = true;
		matchAnyStatus = false;
	}
	
	bool addStatus(const std::string &name)
	{
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
		{
			if (KNOWN_STATUSES[i] == name)
			{
				selected[i] = true;
				matchAnyStatus = false;
				return true;
			}
		}
		return false;
	}

	void setAnyStatus()
	{
		matchAnyStatus = true;
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
			selected[i] = false;
	}
	
	bool allowsStatus(const std::string &name) const
	{
		if (matchAnyStatus)
			return true;
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
			if (selected[i] && KNOWN_STATUSES[i] == name)
				return true;
		return false;
	}

	bool hasStatusFilter() const { return !matchAnyStatus; }

	Worker *getAssignedTo() const
	{
		return assignedTo;
	}

	void setAssignedTo(Worker *w)
	{
		assignedTo = w;
	}
};

#endif
