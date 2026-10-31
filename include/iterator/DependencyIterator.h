#ifndef DEPENDENCYITERATOR_H
#define DEPENDENCYITERATOR_H

#include "Iterator.h"
#include <stack>

/**
 * @class DependencyIterator
 * @brief Iterator that traverses a WorkComponent tree in dependency (depth-first) order.
 *
 * Uses an explicit stack to perform a depth-first traversal of the component hierarchy. 
 * Children are pushed in reverse order so that they are visited in their natural order. 
 * This iterator yields the root first, followed by its descendants
 *
 * @note This iterator does not own the components it traverses; it only references them. 
 * Ownership semantics are determined by the WorkComponent tree itself
 */

class DependencyIterator : public Iterator
{
private:
	/** @brief Stack used to track pending components during traversal */
	std::stack<WorkComponent *> stack;

	/** @brief The most recently returned WorkComponent */
	WorkComponent *current;

	/**
	 * @brief Pushes the children of the given component onto the stack
	 * @param component The component whose children should be pushed
	 *
	 * Children are pushed in reverse order so that the first child is popped (and thus visited) first, preserving natural ordering
	 */
	void pushChildren(WorkComponent *component);

public:
	/**
	 * @brief Constructs a DependencyIterator rooted at the given component
	 * @param root The root of the component tree to traverse
	 *
	 * The constructor initializes the traversal by invoking first()
	 */
	DependencyIterator(WorkComponent *root);

	/** @brief Destructor */
	~DependencyIterator();

	/**
	 * @brief Resets the iterator and returns the first component
	 * @return The first component in dependency order, or nullptr if empty
	 */
	virtual WorkComponent *first() override;

	/**
	 * @brief Advances to and returns the next component
	 * @return The next component or nullptr if the traversal is complete
	 */
	virtual WorkComponent *next() override;

	/**
	 * @brief Checks whether the traversal if completed
	 * @return true if the current is equal to nullptr and the stack is empty
	 */
	virtual bool isDone() override;

	/**
	 * @brief Returns the current component without advancing
	 * @return The current component, or nullptr if none
	 */
	virtual WorkComponent *currentItem() override;
};

#endif
