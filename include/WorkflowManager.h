#ifndef WORKFLOWMANAGER_H
#define WORKFLOWMANAGER_H

#include <ctime>
#include <string>
#include <vector>

#include "command/WorkflowCommand.h"
#include "iterator/FilterCriteria.h"
#include "composite/WorkComponent.h"
#include "strategy/AssignmentStrategy.h"
#include "factory/ItemBuilder.h"
#include "factory/ItemGroupBuilder.h"

class WorkflowManager
{
private:
	WorkComponent *root;
	std::time_t createdAt;
	AssignmentStrategy *assignmentStrategy;
	std::vector<WorkflowCommand *> commandHistory;
	std::vector<Worker *> workers;

	WorkflowManager(const WorkflowManager &) = delete;
	WorkflowManager &operator=(const WorkflowManager &) = delete;

public:
	WorkflowManager(WorkComponent* root);
	~WorkflowManager();

	/**
	 * @brief Move every item to its next state if the item is not blocked.
	 */
	void progress();

	/**
	 * @brief Cancel every item.
	 */
	void cancel();

	/**
	 * @brief Assign the given item to a worker based on the current assignment strategy.
	 *
	 * @param item The item to assign to a selected work.
	 */
	void assignWork(Item *item);

	/**
	 * @brief Create a dependency iterator for traversing the item tree.
	 */
	Iterator *createDependencyIterator();

	/**
	 * @brief Create a priority iterator for traversing the item tree.
	 */
	Iterator *createPriorityIterator();

	/**
	 * @brief Create a filter wrapper iterator for traversing the item tree using the given iterator as the engine for traversal.
	 *
	 * @param iterator The iterator to use internally for traversal.
	 * @param criteria The criteria used to determine which items to skip over.
	 */
	Iterator *createFilteredIterator(Iterator *iterator, FilterCriteria *criteria);

	/**
	 * @brief Select the next available item in the item tree.
	 */
	WorkComponent *selectNextAvailable();

	/**
	 * @brief Execute a command and save it to the command history.
	 *
	 * @param command The command to execute.
	 */
	void executeCommand(WorkflowCommand *command);

	/**
	 * @brief Get the history of executed commands.
	 *
	 * @return Command history.
	 */
	std::vector<WorkflowCommand *> getHistory();

	/**
	 * @brief Get all registered workers.
	 *
	 * @return registered workers.
	 */
	std::vector<Worker *> getWorkers();

	/**
	 * @brief Register a worker to be used by subsystems.
	 *
	 * @param worker The worker to register.
	 */
	void addWorker(Worker *worker);

	/**
	 * @brief Create a builder instance for easily creating items to be added to the item tree.
	 *
	 * @return An item builder.
	 */
	ItemBuilder *createItemBuilder();

	/**
	 * @brief Create a builder instance for easily creating item groups to be added to the item tree.
	 *
	 * @return An item builder.
	 */
	ItemGroupBuilder *createItemGroupBuilder();

	/**
	 * @brief Set the strategy used to assign workers to items.
	 * 
	 * @param strategy The assignment strategy to use.
	 */
	void setAssignmentStrategy(AssignmentStrategy* strategy);
};

#endif
