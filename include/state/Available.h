#ifndef AVAILABLE_H
#define AVAILABLE_H

#include "State.h"

class Available : public State
{
private:
	bool isEscalated;

public:
	Available();

	/**
	 * @brief Advance an available item to its next state.
	 *
	 * @param item The item being advanced.
	 */
	void progress(Item *item);

	/**
	 * @brief Cancel an available item.
	 *
	 * @param item The item being cancelled.
	 */
	void cancel(Item *item);

	/**
	 * @brief Block an available item.
	 *
	 * @param item The item being blocked.
	 */
	void block(Item *item);

	/**
	 * @brief Unblock an available item.
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
	 * @return "AVAILABLE".
	 */
	std::string getName() const override;
};

#endif
