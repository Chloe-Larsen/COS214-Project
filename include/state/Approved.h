#ifndef APPROVED_H
#define APPROVED_H

#include "State.h"

class Approved : public State
{
public:
	Approved();

	/**
	 * @brief Advance an approved item to its next state.
	 *
	 * @param item The item being advanced.
	 */
	void progress(Item *item);

	/**
	 * @brief Cancel an approved item.
	 *
	 * @param item The item being cancelled.
	 */
	void cancel(Item *item);

	/**
	 * @brief Block an approved item.
	 *
	 * @param item The item being blocked.
	 */
	void block(Item *item);

	/**
	 * @brief Unblock an approved item.
	 *
	 * @param item The item being unblocked.
	 */
	void unblock(Item *item);

	/**
	 * @brief Get the state's name.
	 *
	 * @return "APPROVED".
	 */
	std::string getName() const override;
};

#endif
