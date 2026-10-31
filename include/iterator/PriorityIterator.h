#ifndef PRIORITYITERATOR_H
#define PRIORITYITERATOR_H

#include "Iterator.h"
#include <queue>

/**
 * @struct PriorityComparator
 * @brief Comparator ordering WorkComponents by priority (highest first)
 *
 * Uses dynamic_cast to Item to obtain priority; non-Item components are treated as priority 0.
 * Returns true when a has lower priority than b, which makes std::priority_queue pop the highest-priority element first
 */
struct PriorityComparator
{
	/**
	 * @brief Compares two components by priority
	 * @param a First component
	 * @param b Second component
	 * @return true if a has lower priority than b
	 */
	bool operator()(WorkComponent *a, WorkComponent *b) const;
};

/**
 * @class PriorityIterator
 * @brief Iterator that yields components in descending priority order
 *
 * Collects all components in the tree into a priority queue, then pops them one by one so that higher-priority items are returned first
 *
 * @note This iterator does not own the components it traverses; ownership remains with the component tree / root
 */
class PriorityIterator : public Iterator
{
private:
	/** @brief Priority queue of pending components */
	std::priority_queue<WorkComponent *, std::vector<WorkComponent *>, PriorityComparator> queue;

	/** @brief The most recently returned component */
	WorkComponent *current;

	/**
	 * @brief Recursively collects all components in the subtree into the queue
	 * @param node Root of the subtree to collect
	 */
	void collectAll(WorkComponent *node);

public:
	/**
	 * @brief Constructs a PriorityIterator rooted at the given component
	 * @param root Root of the component tree to traverse
	 */
	PriorityIterator(WorkComponent *root);

	/** @brief Deconstructor */
	~PriorityIterator();

	/**
	 * @brief Resets the iterator and returns the highest-priority component
	 * @return The first component in priority order, or nullptr if empty
	 */
	virtual WorkComponent *first() override;
	
	/**
	 * @brief  Advances to and returns the next highest-priority component
	 * @return The next component, or nullptr if traversal is completed
	 */
	virtual WorkComponent *next() override;
	
	/**
	 * @brief Checks whether the priority traversal has finished
	 * @return true if no current item and queue is empty
	 */
	virtual bool isDone() override;
	
	/**
	 * @brief Returns the current commonent without updating
	 * @return The current component, nullptr if none
	 */
	virtual WorkComponent *currentItem() override;
};

#endif
