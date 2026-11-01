#ifndef CANCELLED_H
#define CANCELLED_H

#include "State.h"

class Cancelled : public State
{
public:
	Cancelled();

	/**
	 * @brief Attempt to advance a cancelled item (typically a no-op).
	 *
	 * @param item The item being advanced.
	 */
	void progress(Item *item);

	/**
	 * @brief Cancel an already cancelled item (typically a no-op).
	 *
	 * @param item The item being cancelled.
	 */
	void cancel(Item *item);

	/**
	 * @brief Attempt to block a cancelled item (typically a no-op).
	 *
	 * @param item The item being blocked.
	 */
	void block(Item *item);

	/**
	 * @brief Attempt to unblock a cancelled item (typically a no-op).
	 *
	 * @param item The item being unblocked.
	 */
	void unblock(Item *item);

	/**
	 * @brief Get the state's name.
	 *
	 * @return "CANCELLED".
	 */
	std::string getName() const override;
};

#endif
