#ifndef INPROGRESS_H
#define INPROGRESS_H

#include "State.h"

class InProgress : public State
{
private:
	bool isEscalated;
public:
	InProgress();

	/**
	 * @brief Advance an in-progress item to its next state.
	 *
	 * @param item The item being advanced.
	 */
	void progress(Item *item);

	/**
	 * @brief Cancel an in-progress item.
	 *
	 * @param item The item being cancelled.
	 */
	void cancel(Item *item);

	/**
	 * @brief Block an in-progress item.
	 *
	 * @param item The item being blocked.
	 */
	void block(Item *item);

	/**
	 * @brief Unblock an in-progress item.
	 *
	 * @param item The item being unblocked.
	 */
	void unblock(Item *item);

	/**
	 * @brief Report whether the item is escalated.
	 *
	 * @return True if escalated.
	 */
	bool getEscalated() override;

	/**
	 * @brief Mark the item as escalated.
	 */
	void setEscalated() override;

	/**
	 * @brief Get the state's name.
	 *
	 * @return "IN_PROGRESS".
	 */
	std::string getName() const override;
};

#endif
