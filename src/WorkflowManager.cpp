#include <memory>

#include "WorkflowManager.h"
#include "../include/command/WorkflowCommand.h"
#include "../include/composite/WorkComponent.h"
#include "../include/iterator/DependencyIterator.h"
#include "../include/iterator/PriorityIterator.h"
#include "../include/iterator/Filtered.h"

WorkflowManager::WorkflowManager()
    : root(nullptr), createdAt(std::time(nullptr)),
      routing(nullptr), assignmentStrategy(nullptr) {}

WorkflowManager::~WorkflowManager()
{
    if (root != nullptr)
        delete root;
    if (routing != nullptr)
        delete routing;
    if (assignmentStrategy != nullptr)
        delete assignmentStrategy;
    for (WorkflowCommand *workflowCommand : commandHistory)
        delete workflowCommand;
    for (Worker *worker : workers)
        delete worker;
}

void WorkflowManager::progress()
{
    if (!root)
        throw "The workflow has no root component.";
    root->progress();
}

void WorkflowManager::cancel()
{
    if (!root)
        throw "The workflow has no root component.";
    root->cancel();
}

void WorkflowManager::assignWork(Item *item)
{
    if (!assignmentStrategy)
        throw "No assignment strategy has been provided.";

    // use strategy to get the worker to assign to the work
    Worker *worker = assignmentStrategy->assign(item, this);
    item->assignWorker(worker);
}

Iterator *WorkflowManager::createDependencyIterator()
{
    return new DependencyIterator(root);
}

Iterator *WorkflowManager::createPriorityIterator()
{
    return new PriorityIterator(root);
}

Iterator *WorkflowManager::createFilteredIterator(Iterator *iterator, FilterCriteria *criteria)
{
    return new Filtered(iterator, criteria);
}

WorkComponent *WorkflowManager::selectNextAvailable()
{
    Iterator *iterator = createFilteredIterator(createPriorityIterator(), new FilterCriteria(ItemStatus::AVAILABLE));
    WorkComponent *workComponent = iterator->next();
    delete iterator;
    return workComponent;
}

void WorkflowManager::executeCommand(WorkflowCommand *command)
{
    if (!command)
        throw "Cannot execute null command";

    for (std::size_t i = 0; i < commandHistory.size(); ++i)
        if (commandHistory[i] == command)
            throw "Command has already been executed!";

    command->execute();
    commandHistory.push_back(command); // add to history
}

std::vector<WorkflowCommand *> WorkflowManager::getHistory()
{
    return commandHistory;
}

std::vector<Worker *> WorkflowManager::getWorkers()
{
    return workers;
}

void WorkflowManager::addWorker(Worker *worker)
{
    workers.push_back(worker);
}

ItemBuilder *WorkflowManager::createItemBuilder()
{
    return new ItemBuilder();
}

ItemGroupBuilder *WorkflowManager::createItemGroupBuilder()
{
    return new ItemGroupBuilder();
}
