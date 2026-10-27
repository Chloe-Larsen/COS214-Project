#include "../../include/composite/ItemGroup.h"
#include "../../include/iterator/DependencyIterator.h"
#include "../../include/iterator/PriorityIterator.h"
#include <memory>

// helper detects cycles
static bool containsComponent(WorkComponent *root, WorkComponent *target)
{
    if (root == target)
        return true;
    ItemGroup *group = dynamic_cast<ItemGroup *>(root);
    if (!group)
        return false;
    for (int i = 0; i < group->getChildCount(); ++i)
        if (containsComponent(group->getChild(i), target))
            return true;
    return false;
}

ItemGroup::ItemGroup(std::string name) : WorkComponent(name) {}

ItemGroup::ItemGroup(const ItemGroup &other) : WorkComponent(other)
{
    try
    {
        for (std::size_t i = 0; i < other.children.size(); ++i)
        {
            std::unique_ptr<WorkComponent> child(other.children[i]->clone());
            add(child.get());
            child.release();
        }
    }
    catch (...)
    {
        for (std::size_t i = 0; i < children.size(); ++i)
            delete children[i];
        throw;
    }
}

ItemGroup::ItemGroup() : WorkComponent(nullptr)
{
}

ItemGroup::~ItemGroup()
{
    for (std::size_t i = 0; i < children.size(); ++i)
    {
        delete children[i];
    }
}

void ItemGroup::progress()
{
    for (std::size_t i = 0; i < children.size(); ++i)
        children[i]->progress();
}

void ItemGroup::cancel()
{
    for (std::size_t i = 0; i < children.size(); ++i)
        children[i]->cancel();
}

void ItemGroup::requestReview()
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        children[i]->requestReview();
    }
}

void ItemGroup::add(WorkComponent *child)
{
    if (!child)
        throw std::string("Cannot add a null child.");
    if (containsComponent(this, child))
        throw std::string("This component already belongs in this tree.");
    if (containsComponent(child, this))
        throw std::string("Adding this child creates a cycle.");

    children.push_back(child);
}

void ItemGroup::remove(WorkComponent *child)
{
    for (std::size_t i = 0; i < children.size(); ++i)
    {
        if (children[i] == child)
        {
            children.erase(children.begin() + i);
            return; // Caller owns the detached child
        }
    }
}

ItemGroup *ItemGroup::clone() const
{
    return new ItemGroup(*this);
}

Iterator *ItemGroup::createIterator(std::string type)
{
    if (type == "Dependency" || type == "dependency")
        return new DependencyIterator(this);
    if (type == "Priority" || type == "priority")
        return new PriorityIterator(this);
    throw std::string("Unknown iterator type: " + type);
}

int ItemGroup::getChildCount()
{
    int total = 0;

    for (std::size_t i = 0; i < children.size(); ++i)
    {
        total += children[i]->getChildCount();
    }

    return total;
}

WorkComponent *ItemGroup::getChild(int index) const
{
    if (index < 0 || static_cast<std::size_t>(index) >= children.size())
        return nullptr;
    return children[static_cast<std::size_t>(index)];
}
