#include "WorkflowManager.h"
#include "../include/command/WorkflowCommand.h"
#include "../include/composite/WorkComponent.h"
#include <memory>

WorkflowManager::WorkflowManager()
    : root(nullptr), createdAt(std::time(nullptr)), status("Created"),
      routing(nullptr), assignmentStrategy(nullptr) {}

WorkflowManager::~WorkflowManager()
{
    for (std::size_t i = 0; i < history.size(); ++i)
        delete history[i];
    delete root;
   //strategy pointers
}

void WorkflowManager::progress() {
    if (!root)
        throw std::string("The workflow has no root component.");
    root->progress();
}

void WorkflowManager::cancel() {
    if (!root)
        throw std::string("The workflow has no root component.");
    root->cancel();
    status = "Cancelled";
}

void WorkflowManager::assignWork(Item* item) {
	// TODO - implement WorkflowManager::assignWork
	throw "Not yet implemented";
}

Iterator* WorkflowManager::createIterator() {
	// TODO - implement WorkflowManager::createIterator
	throw "Not yet implemented";
}

WorkComponent* WorkflowManager::selectNextAvailable() {
	// TODO - implement WorkflowManager::selectNextAvailable
	throw "Not yet implemented";
}

void WorkflowManager::executeCommand(WorkflowCommand* command) {
{
    if (!command)
        throw std::string("Cannot execute a null command.");
    for (std::size_t i = 0; i < history.size(); ++i)
        if (history[i] == command)
            throw std::string("This command is already owned.");

    std::unique_ptr<WorkflowCommand> owned(command);
    history.push_back(command);
    try
    {
        command->execute();
    }
    catch (...)
    {
        history.pop_back();
        throw;
    }
    owned.release(); // Successful
}

std::vector<WorkflowCommand*> WorkflowManager::getHistory() {
	return this->history;
}

ItemBuilder* WorkflowManager::createItemBuilder() {
	return new ItemBuilder();
}

ItemGroupBuilder* WorkflowManager::createItemGroupBuilder() {
	return new ItemGroupBuilder();
}
