#ifndef FILTERCRITERIA_H
#define FILTERCRITERIA_H

#include "../chain/Worker.h"
#include <vector>
#include <string>

/**
 * @brief Array of all status names known to the filtering system
 *
 * The order of these strings must remain consistent with any code that indexes into this array (e.g., setStatus(std::size_t)).
 */
static const std::string KNOWN_STATUSES[] = {
	"AVAILABLE",
	"IN_PROGRESS",
	"COMPLETED",
	"APPROVED",
	"BLOCKED",
	"CANCELLED"};

/** @brief Number of entries in KNOWN_STATUSES */
static const std::size_t KNOWN_STATUS_COUNT =
	sizeof(KNOWN_STATUSES) / sizeof(KNOWN_STATUSES[0]);

/**
 * @class FilterCriteria
 * @brief Encapsulates filtering rules for WorkComponents
 *
 * Supports filtering by status (single or multiple) and by assigned Worker.
 * By default, all statuses are allowed (matchAnyStatus == true) and no worker constraint is applied
 */
class FilterCriteria
{
private:
	/** @brief Worker that items must be assigned to, or nullptr for no constraint */
	Worker *assignedTo;

	/** @brief Per-status selection flags indexed parallel to KNOWN_STATUSES */
	bool selected[KNOWN_STATUS_COUNT];

	/** @brief If true, status filtering is disabled and all statuses match */
	bool matchAnyStatus;

public:
	/**
	 * @brief Constructs a FilterCriteria with optional assigned worker
	 * @param p Worker to filter by (default nullptr = no worker filter)
	 *
	 * Initializes all statuses as unselected and matchAnyStatus = true
	 */
	FilterCriteria(Worker *p = nullptr) : matchAnyStatus(true), assignedTo(p)
	{
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
			selected[i] = false;
	}

	/**
	 * @brief Returns whether any status is accepted
	 * @return true if status filtering is disabled
	 */
	bool isMatchAnyStatus() const
	{
		return matchAnyStatus;
	}

	/**
	 * @brief Selects a single status by name, clearing all others
	 * @param name Status name to select
	 * @return true if the name was found and selected; false otherwise
	 */
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

	/**
	 * @brief Selects a single status by index, clearing all others
	 * @param index Index into KNOWN_STATUSES. Out-of-range indices are ignored
	 */
	void setStatus(std::size_t index)
	{
		if (index >= KNOWN_STATUS_COUNT)
			return;
		for (std::size_t j = 0; j < KNOWN_STATUS_COUNT; ++j)
			selected[j] = false;
		selected[index] = true;
		matchAnyStatus = false;
	}

	/**
	 * @brief Adds a status to the set of accepted statuses
	 * @param name Status name to add
	 * @return true if the name was found and added; false otherwise
	 */
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

	/**
	 * @brief Resets the filter to accept any status
	 */
	void setAnyStatus()
	{
		matchAnyStatus = true;
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
			selected[i] = false;
	}

	/**
	 * @brief Checks whether a given status is allowed by this filter
	 * @param name Status name to test
	 * @return true if matchAnyStatus is set or the status is selected
	 */
	bool allowsStatus(const std::string &name) const
	{
		if (matchAnyStatus)
			return true;
		for (std::size_t i = 0; i < KNOWN_STATUS_COUNT; ++i)
			if (selected[i] && KNOWN_STATUSES[i] == name)
				return true;
		return false;
	}

	/**
	 * @brief Returns whether a status filter is currently active
	 * @return true if at least one status is selected
	 */
	bool hasStatusFilter() const
	{
		return !matchAnyStatus;
	}

	/**
	 * @brief Returns the worker used for assignment filtering
	 * @return Pointer to the assigned worker, or nullptr if no filter
	 */
	Worker *getAssignedTo() const
	{
		return assignedTo;
	}

	/**
	 * @brief Sets the worker used for assignment filtering
	 * @param w Worker to filter by (nullptr disables worker filtering)
	 */
	void setAssignedTo(Worker *w)
	{
		assignedTo = w;
	}
};

#endif
