#ifndef ITERATOR_H
#define ITERATOR_H

class WorkComponent;

/**
 * @class Iterator
 * @brief Abstract base class for traversals over WorkComponent trees
 *
 * Defines the standard traversal interface: first(), next(), isDone(), and currentItem(). 
 * Concrete subclasses implement specific traversal strategies (dependency-based, priority-based, filtered, etc.)
 *
 * @note The base class takes ownership of the root and deletes it in the destructor
 */
class Iterator
{
protected:
	/** @brief Root of the traversal (owned by the iterator) */
	WorkComponent *root;

public:
	/**
	 * @brief Constructs an Iterator with the given root
	 * @param root Root component (ownership transferred)
	 */
	Iterator(WorkComponent *root);

	/** @brief Virtual destructor. Deletes the root if non-null */
	virtual ~Iterator();

	/**
	 * @brief Resets the iterator and returns the first component
	 * @return The first component, or nullptr if empty
	 */
	virtual WorkComponent *first() = 0;

	/**
	 * @brief Advances and returns the next component
	 * @return The next component, or nullptr if traversal is complete
	 */
	virtual WorkComponent *next() = 0;

	/**
	 * @brief Reports whether the traversal has finished
	 * @return true if no more components remain
	 */
	virtual bool isDone() = 0;

	/**
	 * @brief Returns the current component without advancing
	 * @return The current component, or nullptr if empty
	 */
	virtual WorkComponent *currentItem() = 0;
};

#endif