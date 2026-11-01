#ifndef FILTERED_H
#define FILTERED_H

#include "Iterator.h"
#include "FilterCriteria.h"

/**
 * @class Filtered
 * @brief Iterator decorator that filters items from an inner iterator
 *
 * Wraps another Iterator and only yields WorkComponents that satisfy the supplied FilterCriteria. 
 * Non-matching components are skipped transparently
 *
 * @note Filtered takes ownership of the inner iterator and deletes it in its destructor. 
 * It does NOT take ownership of the FilterCriteria
 */
class Filtered : public Iterator
{
private:
	/** @brief The wrapped iterator supplying candidate components */
	Iterator *inner;

	/** @brief The criteria used to accept or reject candidates */
	FilterCriteria *criteria;

	/** @brief The most recently matched component */
	WorkComponent *current;

	/**
	 * @brief Determines whether a component satisfies the criteria
	 * @param component Candidate component to test
	 * @return true if the component matches; false otherwise
	 */
	bool matches(WorkComponent *component) const;

	/**
	 * @brief Advances the inner iterator to the next matching component
	 *
	 * Sets current to the next matching component, or nullptr if none remain
	 */
	void advanceToNextMatch();

public:
	/**
	 * @brief Constructs a Filtered iterator
	 * @param inner Inner iterator to wrap (ownership transferred)
	 * @param c FilterCriteria to apply (not owned)
	 */
	Filtered(Iterator *inner, FilterCriteria *c);

	/** @brief Destructor Deletes the inner iterator */
	~Filtered();

	/**
	 * @brief Resets and returns the first matching component
	 * @return The first matching component, or nullptr if none
	 */
	virtual WorkComponent *first() override;

	/**
	 * @brief Advances to and returns the next matching component
	 * @return The next matching component, or nullptr if none
	 */
	virtual WorkComponent *next() override;

	/**
	 * @brief Checks whether the filtered traversal has finished
	 * @return true if no current matching component remains
	 */
	virtual bool isDone() override;

	/**
	 * @brief Returns the current matching component
	 * @return The current component, or nullptr if none
	 */
	virtual WorkComponent *currentItem() override;
};

#endif
