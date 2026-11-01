#ifndef COMPLETED_H
#define COMPLETED_H

#include "State.h"

class Completed : public State
{
public:
	Completed();

	/**
	 * @brief Attempt to advance a completed item (typically a no-op).
	 *
	 * @param item The item being advanced.
	 */
	void progress(Item *item);

	/**
	 * @brief Attempt to cancel a completed item (typically a no-op).
	 *
	 * @param item The item being cancelled.
	 */
	void cancel(Item *item);

	/**
	 * @brief Attempt to block a completed item (typically a no-op).
	 *
	 * @param item The item being blocked.
	 */
	void block(Item *item);

	/**
	 * @brief Attempt to unblock a completed item (typically a no-op).
	 *
	 * @param item The item being unblocked.
	 */
	void unblock(Item *item);

	/**
	 * @brief Get the state's name.
	 *
	 * @return "COMPLETED".
	 */
	std::string getName() const override;
};

#endif
