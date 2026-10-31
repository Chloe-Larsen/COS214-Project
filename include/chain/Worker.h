/**
 * @file Worker.h
 * @brief Defines a worker who can review items and participate in an approval chain.
 */

#ifndef WORKER_H
#define WORKER_H

#include "../composite/Item.h"
#include "../composite/WorkComponent.h"
#include <functional>
#include <string>
#include <vector>

class Item;
class WorkComponent;

/**
 * @class Worker
 * @brief Represents a worker who can review, approve, reject, and escalate workflow items.
 *
 * Worker implements the Handler role in the Chain of Responsibility pattern.
 * Each worker can review an item and either approve it, reject it, or forward
 * the approval request to their supervisor when additional approval levels are required.
 *
 * Workers also maintain a reference to a workflow root, which allows their
 * workload to be calculated by traversing the workflow using an iterator.
 */
class Worker
{
	private:
		/**
		 * @brief Function used to determine whether a worker approves an item.
		 *
		 * The decision policy receives an Item pointer and returns true if the
		 * worker approves the item or false if the worker rejects it.
		 */
		typedef std::function<bool(Item *)> DecisionPolicy;

		/** @brief Unique identifier of the worker. */
		std::string id;

		/** @brief Full name of the worker. */
		std::string name;

		/** @brief Email address of the worker. */
		std::string email;

		/** @brief Job role or position held by the worker. */
		std::string role;

		/** @brief The worker's direct supervisor, who receives forwarded approval requests. */
		Worker *supervisor;

		/** @brief Workers who report directly to this worker. */
		std::vector<Worker *> subordinates;

		/** @brief Root of the workflow tree used to calculate this worker's workload. */
		WorkComponent *workflowRoot;

		/** @brief The policy used by this worker to approve or reject items. */
		DecisionPolicy policy;

		/** @brief Items that have been escalated to this worker for further attention. */
		std::vector<Item *> escalatedQueue;

		/** @brief Maximum number of approval levels allowed in a single chain. */
		static const int MAX_CHAIN_DEPTH = 64;

	public:
	/**
		 * @brief Constructs a worker and optionally connects them to a supervisor.
		 * @param id Unique identifier of the worker.
		 * @param name Name of the worker.
		 * @param role Role or position held by the worker.
		 * @param supervisor The worker's direct supervisor. Defaults to NULL.
		 *
		 * If a supervisor is provided, this worker is automatically added
		 * to that supervisor's subordinate list.
		 */
		Worker(std::string id, std::string name, std::string role, Worker *supervisor = NULL);

		/**
		 * @brief Reviews an item as part of the approval chain.
		 * @param item The item requiring approval.
		 * @param depth Number of approval levels remaining.
		 * @return true if the required approval chain succeeds; false if rejected,
		 *         escalated, or the request is invalid.
		 *
		 * The worker applies their decision policy. If the item is rejected,
		 * the chain terminates immediately. If approved but more approval levels
		 * are required, the request is forwarded to the supervisor.
		 * If no supervisor is available, the item is escalated.
		 *
		 * @note The depth parameter prevents unlimited recursive forwarding.
		 */
		bool review(Item *item, int depth);

		/**
		 * @brief Adds an item to this worker's escalation queue.
		 * @param item The item requiring further attention.
		 *
		 * The item is marked as escalated and assigned to this worker.
		 * Duplicate items are not added to the queue.
		 */
		void handleEscalation(Item *item);

		/**
		 * @brief Calculates the number of workflow items assigned to this worker.
		 * @return The number of items assigned to this worker.
		 *
		 * Uses DependencyIterator to traverse the workflow tree and count
		 * Item objects whose assigned worker is this worker.
		 */
		int getWorkload();


		/**
		 * @brief Sets the root of the workflow tree used for workload calculation.
		 * @param root Pointer to the root WorkComponent of the workflow.
		 */
		void setWorkflowRoot(WorkComponent *root);

		/**
		 * @brief Changes this worker's direct supervisor.
		 * @param s Pointer to the new supervisor.
		 */
		void setSupervisor(Worker *s);

		/**
		 * @brief Adds a worker to this worker's subordinate list.
		 * @param w Pointer to the worker who reports to this worker.
		 */
		void addSubordinate(Worker *w);

		/**
		 * @brief Sets the decision policy used when reviewing items.
		 * @param p Function that returns true to approve or false to reject.
		 */
		void setDecisionPolicy(DecisionPolicy p);


		/** @brief Gets this worker's direct supervisor. */
		Worker *getSupervisor() const;

		/** @brief Gets this worker's unique identifier. */
		const std::string &getId() const;

		/** @brief Gets this worker's name. */
		const std::string &getName() const;

		/** @brief Gets this worker's email address. */
		const std::string &getEmail() const;

		/** @brief Gets this worker's role. */
		const std::string &getRole() const;

		/**
		 * @brief Gets the list of items escalated to this worker.
		 * @return Constant reference to the escalation queue.
		 */
		const std::vector<Item *> &getEscalatedQueue() const;
};

#endif