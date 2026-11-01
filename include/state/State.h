#ifndef STATE_H
#define STATE_H

#include <string>

class Item;

class State
{
public:
	State();
	virtual ~State();

	/**
	 * @brief Advance the item to its next lifecycle state.
	 *
	 * @param item The item whose state is changing.
	 */
	virtual void progress(Item *item) = 0;

	/**
	 * @brief Cancel the item from this state.
	 *
	 * @param item The item being cancelled.
	 */
	virtual void cancel(Item *item) = 0;

	/**
	 * @brief Block the item from this state.
	 *
	 * @param item The item being blocked.
	 */
	virtual void block(Item *item) = 0;

	/**
	 * @brief Unblock the item from this state.
	 *
	 * @param item The item being unblocked.
	 */
	virtual void unblock(Item *item) = 0;

	/**
	 * @brief Report whether the item is escalated in this state.
	 *
	 * @return True if escalated; false by default.
	 */
	virtual bool getEscalated() { return false; }

	/**
	 * @brief Mark the item as escalated in this state.
	 */
	virtual void setEscalated() {};

	/**
	 * @brief Get the canonical name of this state.
	 *
	 * @return The state's lifecycle name.
	 */
	virtual std::string getName() const = 0;
};

#endif