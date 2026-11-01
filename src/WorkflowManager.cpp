#include <memory>

#include "WorkflowManager.h"
#include "../include/command/Assign.h"
#include "../include/command/Cancel.h"
#include "../include/command/Progress.h"
#include "../include/command/WorkflowCommand.h"
#include "../include/composite/WorkComponent.h"
#include "../include/iterator/DependencyIterator.h"
#include "../include/iterator/PriorityIterator.h"
#include "../include/iterator/Filtered.h"

WorkflowManager::WorkflowManager(WorkComponent *root)
    : root(root), createdAt(std::time(nullptr)),
      assignmentStrategy(nullptr) {}

WorkflowManager::~WorkflowManager()
{
    if (root != nullptr)
        delete root;
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
    executeCommand(new Progress(root));
}

void WorkflowManager::cancel()
{
    if (!root)
        throw "The workflow has no root component.";
    executeCommand(new Cancel(root));
}

void WorkflowManager::assignWork(Item *item)
{
    if (!assignmentStrategy)
        throw "No assignment strategy has been provided.";

    // use strategy to get the worker to assign to the work
    Worker *worker = assignmentStrategy->assign(item, this);

    if (worker != nullptr)
        executeCommand(new Assign(item, worker));
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
    FilterCriteria *fc = new FilterCriteria();
    fc->setStatus("AVAILABLE");
    Iterator *iterator = createFilteredIterator(createPriorityIterator(), fc);
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
    worker->setWorkflowRoot(root);
}

ItemBuilder *WorkflowManager::createItemBuilder()
{
    return new ItemBuilder();
}

ItemGroupBuilder *WorkflowManager::createItemGroupBuilder()
{
    return new ItemGroupBuilder();
}

void WorkflowManager::setAssignmentStrategy(AssignmentStrategy *strategy)
{
    if (assignmentStrategy != nullptr)
        delete assignmentStrategy;
    assignmentStrategy = strategy;
}