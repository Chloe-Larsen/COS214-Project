#include "../../include/composite/Item.h"
#include "../../include/chain/Worker.h"
#include "../../include/state/State.h"
#include "../../include/state/Available.h"
#include "../../include/state/Blocked.h"
#include "../../include/state/Approved.h"
#include "../../include/iterator/DependencyIterator.h"
#include "../../include/iterator/PriorityIterator.h"
#include <cstdio>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <ctime>

static std::time_t parseItemDeadline(const std::string &text)
{
    if (text.empty())
        return 0;
    if (text.size() != 10 && text.size() != 19)
        throw std::string("Use YYYY-MM-DD for the deadline.");

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        char separator = 0;
        if (i == 4 || i == 7)
            separator = '-';
        if (i == 10)
            separator = ' ';
        if (i == 13 || i == 16)
            separator = ':';
        if ((separator && text[i] != separator) ||
            (!separator && (text[i] < '0' || text[i] > '9')))
            throw std::string("Invalid deadline: " + text);
    }

    std::tm date = {};
    int year = 0;
    for (int i = 0; i < 4; ++i)
        year = year * 10 + text[i] - '0';
    date.tm_year = year - 1900;
    date.tm_mon = (text[5] - '0') * 10 + text[6] - '0' - 1;
    date.tm_mday = (text[8] - '0') * 10 + text[9] - '0';
    if (text.size() == 19)
    {
        date.tm_hour = (text[11] - '0') * 10 + text[12] - '0';
        date.tm_min = (text[14] - '0') * 10 + text[15] - '0';
        date.tm_sec = (text[17] - '0') * 10 + text[18] - '0';
    }
    date.tm_isdst = -1;
    const std::tm requested = date;
    std::time_t result = std::mktime(&date); // mktime normalises dates
    if (result == static_cast<std::time_t>(-1) ||
        date.tm_year != requested.tm_year || date.tm_mon != requested.tm_mon ||
        date.tm_mday != requested.tm_mday || date.tm_hour != requested.tm_hour ||
        date.tm_min != requested.tm_min || date.tm_sec != requested.tm_sec)
        throw std::string("Invalid deadline: " + text);
    return result;
}

Item::Item(std::string name, std::string deadline, int priority, Worker *assignedTo)
    : WorkComponent(name), deadline(parseItemDeadline(deadline)), priority(priority),
      assignedTo(assignedTo), currentState(new Available()), dependencies(0),
      rejected(false) {}

// PROTOTYPE: copy configuration, reset runtime state so instances are isolated.
Item::Item(const Item &other) : WorkComponent(other), deadline(other.deadline), priority(other.priority), assignedTo(other.assignedTo), currentState(NULL), dependencies(other.dependencies), rejected(false)
{
    currentState = dependencies > 0 ? new Blocked()
                                    : static_cast<State *>(new Available());
}

Item::Item() : WorkComponent(nullptr), deadline(parseItemDeadline(nullptr)), priority(-1),
               assignedTo(nullptr), currentState(nullptr), dependencies(0),
               rejected(false) {}

Item *Item::clone() const
{
    return new Item(*this);
}

Item::~Item()
{
    assignWorker(nullptr);
    delete currentState;
}

void Item::progress()
{
    if (currentState != NULL)
    {
        currentState->progress(this);
    }
    if (dependencies != 0)
        throw std::string("Item still has unresolved dependencies: " + name);
    if (!currentState)
        throw std::string("Set a State before progressing item: " + name);
    currentState->progress(this);
}

void Item::cancel()
{
    if (!currentState)
        throw std::string("Set a State before cancelling item: " + name);
    currentState->cancel(this);
}

void Item::reject(std::string reason)
{
    rejectionReason = reason;
    rejected = true;
}

void Item::incrementDependencies()
{
    ++dependencies;
    try
    {
        if (dependencies == 1 && currentState)
            currentState->block(this);
    }
    catch (...)
    {
        --dependencies;
        throw;
    }
}

void Item::decrementDependencies()
{
    if (dependencies == 0)
        return;
    --dependencies;
    try
    {
        if (dependencies == 0 && currentState)
            currentState->unblock(this);
    }
    catch (...)
    {
        ++dependencies;
        throw;
    }
}

void Item::requestReview()
{
    rejected = false;
    rejectionReason.clear();
    if (!assignedTo || !assignedTo->getSupervisor())
    {
        reject("No reviewer available for item " + name);
        return;
    }
    if (assignedTo->getSupervisor()->review(this, 0))
    {
        setState(new Approved());
    }
}

bool Item::isEscalated() const
{
    return currentState->getEscalated();
}

bool Item::isRejected() const
{
    return rejected;
}

const std::string &Item::getRejectionReason() const
{
    return rejectionReason;
}

void Item::escalate()
{
    if (currentState)
        currentState->setEscalated();

    Worker *superior = assignedTo ? assignedTo->getSupervisor() : nullptr;
    if (superior != nullptr)
        superior->handleEscalation(this);
}

Worker *Item::getWorker()
{
    return assignedTo;
}

void Item::assignWorker(Worker *person)
{
    assignedTo = person;
}

State *Item::getState()
{
    return currentState;
}

void Item::setState(State *state)
{
    if (!state)
        throw std::string("An item's State cannot be null.");
    if (currentState == state)
        return;
    State *previous = currentState;
    currentState = state;

    delete previous;
}

int Item::getPriority()
{
    return this->priority;
}

void Item::setPriority(int priority)
{
    this->priority = priority;
}

Iterator *Item::createIterator(std::string type)
{
    if (type == "Dependency" || type == "dependency")
        return new DependencyIterator(this);

    if (type == "Priority" || type == "priority")
        return new PriorityIterator(this);

    throw std::string("Unknown iterator type: " + type);
}

int Item::getChildCount()
{
    return 1;
}

std::string Item::getStatus() const
{
    return currentState ? currentState->getName() : "UNINITIALISED";
}

void Item::setDeadline(std::time_t deadline)
{
    this->deadline = deadline;
}

int Item::getDependencies() const
{
    return dependencies;
}

std::time_t Item::getDeadline() const
{
    return deadline;
}