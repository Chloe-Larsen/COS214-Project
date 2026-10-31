/**
 * @file Item.h
 * @brief Leaf work component with state, assignment and dependency data.
 * @ingroup tividy_composite
 */

#ifndef ITEM_H
#define ITEM_H

#include "WorkComponent.h"
#include <ctime>
#include <memory>
#include "../state/State.h"

class Worker;
class State;

/**
 * @brief Leaf participant in the Composite and context for the State pattern.
 * @details Owns its current State. Uses one Worker object.
 * @ingroup tividy_composite
 */
class Item : public WorkComponent
{
private:
	/**
	 * @brief Deadline in local time; zero means no deadline.
	 */
	std::time_t deadline;
	/**
	 * @brief Manually entered signed priority; larger values sort first.
	 */
	int priority;
	/**
	 * @brief Pointer sole assignee, or null for unassigned work.
	 */
	Worker *assignedTo;
	/**
	 * @brief Exclusively owned current lifecycle state.
	 */
	std::unique_ptr<State> currentState;
    /**
     * @brief Exclusively owned active state retained while BLOCKED.
     */
    std::unique_ptr<State> resumeState;	
	/**
	 * @brief Number of unresolved prerequisite Items.
	 */
	int dependencies;
	/**
	 * @brief Whether a rejection has been recorded and not made by progression or a review request.
	 */
	bool rejected;
    /**
     * @brief Persistent flag indicating that the item has been escalated.
     */
    bool escalated;
	/**
	 * @brief Text describing why Item has been rejected.
	 */
	std::string rejectionReason;
	/**
	 * @brief Prevent assignment of state-owning items.
	 * @details Intentionally private and undefined; use clone() for prototype copies.
	 */
	Item &operator=(const Item &);

public:
	/**
	 * @brief Create an AVAILABLE item with no unresolved dependencies.
	 * @param[in] name Initial display name.
	 * @param[in] deadline Empty for no deadline, or a valid local date in
	 * YYYY-MM-DD or YYYY-MM-DD HH:MM:SS format.
	 * @param[in] priority Manually entered signed priority.
	 * @param[in] assignedTo Borrowed worker, or null to leave the item unassigned.
	 * @throws std::string If deadline syntax or the calendar date is invalid.
	 */
	Item(std::string name, std::string deadline, int priority, Worker *assignedTo);
	/**
	 * @brief Copy configuration while resetting lifecycle execution.
	 * @param[in] other Source item whose name, supported observers, deadline,
	 * priority, borrowed worker and dependency count are copied.
	 * @details Starts AVAILABLE, or BLOCKED with an AVAILABLE resume state when
	 * dependencies remain. Rejection and escalation flags are reset.
	 * @note The worker is shared by a pointer.
	 */
	Item(const Item &other);
	/**
	 * @brief Create an empty AVAILABLE item for builder use.
	 * @details Uses no deadline, priority zero, no worker and no dependencies.
	 */
	Item();
	/**
	 * @brief Release owned lifecycle states.
	 * @details Does not delete the borrowed worker or publish an unassignment event.
	 */
	~Item();
	/**
	 * @brief Perform exactly one transition through the current State.
	 * @throws std::runtime_error If dependencies remain or the state is COMPLETED
	 * or CANCELLED.
	 * @details Clears the rejection flag and reason before delegation. State and Observer operations handle exceptions.
	 */
	void progress() override;
	/**
	 * @brief Delegate cancellation to the current State.
	 * @throws std::runtime_error If the item is already COMPLETED or CANCELLED.
	 * @details A transition out of BLOCKED clears the saved resume state.
	 */
	void cancel() override;
	/**
	 * @brief Record rejection and publish REJECTED.
	 * @param[in] reason Nonempty, non-whitespace rejection text.
	 * @throws std::invalid_argument If reason is blank.
	 * @details Sets a flag and reason without replacing the lifecycle State.
	 */
	void reject(std::string reason);
	/**
	 * @brief Register one additional unresolved prerequisite.
	 * @details The first prerequisite blocks AVAILABLE or IN_PROGRESS work while
	 * retaining that exact state for resumption, and publishes CHANGED. Other
	 * lifecycle states retain their state while the dependency count increases.
	 */
	void incrementDependencies();
	/**
	 * @brief Resolve one outstanding prerequisite.
	 * @details Zero is a no-op. Resolving the last prerequisite on a BLOCKED item
	 * restores its saved state, defaults to AVAILABLE if none was saved, and emits
	 * CHANGED.
	 */
	void decrementDependencies();
	/**
	 * @brief Read the unresolved prerequisite count.
	 * @return The current nonnegative dependency count.
	 */
	int getDependencies() const;
	/**
	 * @brief Request one approval level from the assignee's immediate superior.
	 * @throws std::runtime_error If blocked, not IN_PROGRESS, unassigned, or lacking
	 * a superior.
	 * @details Clears previous rejection data and invokes Worker::review() with
	 * depth one. Approval transitions to APPROVED; a declined review leaves the
	 * lifecycle state in place and records the rejection through the worker.
	 */
	void requestReview() override;
	/**
	 * @brief Query the item's escalation flag.
	 * @return True after escalation or restoration of a persisted escalation flag.
	 */
	bool isEscalated() const;
	/**
	 * @brief Query the item's rejection flag.
	 * @return True when the current rejection flag is set.
	 */
	bool isRejected() const;
	/**
	 * @brief Access the stored rejection text.
	 * @return Borrowed reference to the current reason, possibly empty.
	 */
	const std::string &getRejectionReason() const;
	/**
	 * @brief Escalate one level to the current worker's immediate superior.
	 * @throws std::runtime_error If the item is unassigned or has no superior.
	 * @details Sets the escalation flag, asks the superior to queue and assign the
	 * item, then publishes ESCALATED.
	 */
	void escalate();
	/**
	 * @brief Access the sole assignee.
	 * @return Borrowed worker pointer, or null when unassigned.
	 */
	Worker *getWorker();
	/**
	 * @brief Replace or clear the sole borrowed assignee.
	 * @param[in] person Worker to assign, or null to unassign.
	 * @details Emits ASSIGNED for a changed non-null assignee or CHANGED when
	 * unassigning. Reassigning the same pointer mean owwnership is not transferred.
	 */
	void assignWorker(Worker *person);
	/**
	 * @brief Access the current lifecycle state.
	 * @return Borrowed non-null State pointer.
	 */
	State *getState();
        /**
         * @brief Describe the state retained while the item is blocked.
         * @return AVAILABLE or IN_PROGRESS when retained; otherwise an empty string.
         */
        std::string getResumeStatus() const;
        /**
         * @brief Restore persisted execution fields without replaying historical events.
         * @param[in] state Canonical lifecycle name, or empty to default to AVAILABLE.
         * @param[in] pending Nonnegative number of unresolved prerequisites.
         * @param[in] rejected Rejection flag to restore.
         * @param[in] escalated Escalation flag to restore.
         * @param[in] reason Stored reason; nonempty when rejected is true.
         * @param[in] resume Saved AVAILABLE or IN_PROGRESS state for a BLOCKED item;
         * empty defaults to AVAILABLE and is required for non-BLOCKED input states.
         * @throws std::invalid_argument If a state, dependency count, rejection reason
         * or resume-state combination is invalid.
         * @details BLOCKED requires pending prerequisites. AVAILABLE or IN_PROGRESS
         * input with pending prerequisites is restored as BLOCKED with that state saved.
         * Does not change the name, deadline, priority or assignee.
         */
        void restoreSnapshot(const std::string &state, int pending, bool rejected, bool escalated,
                         const std::string &reason, const std::string &resume = "");
	/**
	 * @brief Transfer ownership of a non-null lifecycle state.
	 * @param[in] state State that the item will exclusively own.
	 * @throws std::invalid_argument If state is null.
	 * @details The current pointer is not available. Other replacements destroy the old
	 * state, clear the resume state when leaving BLOCKED, and publish APPROVED,
	 * COMPLETED or CHANGED if the state name changes.
	 * @pre Except for the current pointer, state is not owned elsewhere.
	 * @note Observer delivery happens after the state replacement.
	 */
	void setState(State *state);
	/**
	 * @brief Read manually entered priority.
	 * @return The signed priority value; larger values sort first.
	 */
	int getPriority();
	/**
	 * @brief Store priority and publish a change when it differs.
	 * @param[in] priority New manually entered signed priority.
	 * @details Emits CHANGED for a changed value.
	 */
	void setPriority(int priority);
	/**
	 * @brief Read the stored deadline.
	 * @return Local-time timestamp, with zero conventionally meaning no deadline.
	 */
	std::time_t getDeadline() const;
	/**
	 * @brief Create a caller-owned prototype copy of this leaf.
	 * @return Newly allocated Item with the copy constructor's reset semantics.
	 */
	Item *clone() const override;
	/**
	 * @brief Create an iterator whose root is this leaf.
	 * @param[in] type "Dependency", "dependency", "Priority" or "priority".
	 * @return Newly allocated iterator owned by the caller; the Item remains borrowed.
	 * @throws std::invalid_argument If type is unknown.
	 * @pre The item outlives the returned iterator.
	 */
	Iterator *createIterator(std::string type) override;
	/**
	 * @brief Contribute one leaf to composite aggregation.
	 * @return One; this leaf owns no child components.
	 */
	int getChildCount() override;
	/**
	 * @brief Read the current canonical lifecycle name.
	 * @return AVAILABLE, IN_PROGRESS, APPROVED, COMPLETED, BLOCKED or CANCELLED.
	 */
	std::string getStatus() const;
	/**
	 * @brief Store an already parsed deadline timestamp.
	 * @param[in] deadline Local-time timestamp; zero means no deadline.
	 */
	void setDeadline(std::time_t deadline);
};

#endif
