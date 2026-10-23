#include "../../include/composite/ItemGroup.h"

ItemGroup::ItemGroup(std::string id, std::string name, std::string status) : WorkComponent(id, name, status)
{
	// TODO - implement ItemGroup::ItemGroup
	throw "Not yet implemented";
}

ItemGroup::ItemGroup(ItemGroup &other) : WorkComponent(other)
{
	// TODO - implement ItemGroup::ItemGroup
	throw "Not yet implemented";
}

ItemGroup::~ItemGroup()
{
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
	// TODO - implement ItemGroup::requestReview
	throw "Not yet implemented";
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
