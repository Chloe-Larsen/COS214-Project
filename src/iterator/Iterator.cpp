#include "../../include/iterator/Iterator.h"

/**
 * @brief Constructs an Iterator with the given root
 * @param root Root component (ownership transferred)
 */
Iterator::Iterator(WorkComponent *root) : root(root)
{
}

/** @brief Virtual destructor. Deletes the root if non-null */
Iterator::~Iterator()
{
	if (root != nullptr)
		delete root;
}