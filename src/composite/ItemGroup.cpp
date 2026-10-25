#include "../../include/composite/ItemGroup.h"

ItemGroup::ItemGroup(std::string id, std::string name, std::string status) : WorkComponent(id, name, status)
{
	// TODO - implement ItemGroup::ItemGroup
	throw "Not yet implemented";
}

ItemGroup::ItemGroup(ItemGroup &other) : WorkComponent(other)
{
	for(size_t i = 0; i < other.children.size(); ++i)
	{
		children.push_back(other.children[i]->clone()); // recursive deep copy
	}
}

ItemGroup::~ItemGroup()
{
	for(size_t i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}
}

void ItemGroup::progress()
{
	// TODO - implement ItemGroup::progress
	throw "Not yet implemented";
}

void ItemGroup::cancel()
{
	// TODO - implement ItemGroup::cancel
	throw "Not yet implemented";
}

void ItemGroup::requestReview()
{
	for(size_t i = 0; i < children.size(); ++i)
	{
		children[i]->requestReview();
	}
}

void ItemGroup::add(WorkComponent *child)
{
	// TODO - implement ItemGroup::add
	throw "Not yet implemented";
}

void ItemGroup::remove(WorkComponent *child)
{
	// TODO - implement ItemGroup::remove
	throw "Not yet implemented";
}

ItemGroup *ItemGroup::clone()
{
	// TODO - implement ItemGroup::clone
	throw "Not yet implemented";
}
