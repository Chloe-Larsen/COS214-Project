#include "../../include/iterator/Iterator.h"

Iterator::Iterator(WorkComponent *root) : root(root)
{	
}

Iterator::~Iterator()
{
	if(root != nullptr)
		delete root;
}