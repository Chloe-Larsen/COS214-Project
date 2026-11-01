#ifndef BLOCKED_H
#define BLOCKED_H

#include "State.h"

class Blocked : public State
{
private:
	bool isEscalated;

public:
	Blocked();

	/**
	 * @brief Attempt to advance a blocked item (typically a no-op).
	 *
	 * @param item The item being advanced.
	 */
	void progress(Item *item);

	/**
	 * @brief Cancel a blocked item.
	 *
	 * @param item The item being cancelled.
	 */
	void cancel(Item *item);

	/**
	 * @brief Block an already blocked item (typically a no-op).
	 *
	 * @param item The item being blocked.
	 */
	void block(Item *item);

	/**
	 * @brief Unblock the item, restoring its previous state.
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
	 * @return "BLOCKED".
	 */
	std::string getName() const override;
};

#endif
