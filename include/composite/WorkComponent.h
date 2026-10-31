/**
 * @file WorkComponent.h
 * @brief Shared interface for individual work and nested work groups.
 * @ingroup tividy_composite
 */

#ifndef WORKCOMPONENT_H
#define WORKCOMPONENT_H

#include <string>

#include "../observer/EventSource.h"

class Iterator;

/**
 * @brief Abstract Component participant shared by Item and ItemGroup.
 * @details Defines uniform lifecycle, traversal and prototype operations, and
 * inherits event publication from EventSource. An Item is a leaf; an ItemGroup
 * owns child components and forwards operations to them.
 * @ingroup tividy_composite
 */
class WorkComponent : public EventSource
{
protected:	
	/**
	 * @brief Owned component display name.
	 */
	std::string name;	

public:
	/**
	 * @brief Initialise the component's display name.
	 * @param[in] name Initial name; builders may begin with an empty name.
	 * @note Workflow-level name validation is performed by the application service.
	 */
	WorkComponent(std::string name);
	/**
	 * @brief Copy the name and clone supported observers.
	 * @param[in] other Component supplying the name and observer configuration.
	 * @details EventSource owns each non-null observer clone; observers whose
	 * clone() returns null are omitted.
	 */
	WorkComponent(const WorkComponent &other);
	/**
	 * @brief Destroy the component through its polymorphic interface.
	 * @details EventSource releases its owned observers. Derived classes release
	 * their own state and children.
	 */
	virtual ~WorkComponent();
	/**
	 * @brief Progress this leaf or forward progression to grouped children.
	 * @details Concrete lifecycle checks and exception behaviour belong to the
	 * derived component.
	 */
	virtual void progress() = 0;
	/**
	 * @brief Cancel this leaf or forward cancellation to grouped children.
	 * @details Exceptions from a concrete component propagate to the caller.
	 */
	virtual void cancel() = 0;
	/**
	 * @brief Request review of this leaf or the group's children.
	 * @details Eligibility checks and approval decisions are handled by the core.
	 */
	virtual void requestReview() = 0;
	/**
	 * @brief Create an iterator over this component's tree.
	 * @param[in] type "Dependency", "dependency", "Priority" or "priority".
	 * @return A newly allocated iterator owned by the caller; components are borrowed.
	 * @pre This component and its traversed descendants outlive the iterator.
	 * @note Item throws std::invalid_argument for an unknown type; ItemGroup throws
	 * std::string, matching the existing implementations.
	 */
	virtual Iterator *createIterator(std::string type) = 0;
	/**
	 * @brief Return this subtree's recursive leaf count.
	 * @return One for an Item; the sum of child leaf counts for an ItemGroup.
	 * @note This is not the direct-child count. Use ItemGroup::getDirectChildCount()
	 * when indexing immediate children.
	 */
	virtual int getChildCount() = 0;
	/**
	 * @brief Create a caller-owned prototype copy.
	 * @return Newly allocated component owned by the caller.
	 * @details Each concrete class defines its copy semantics. Item copies
	 * configuration and resets execution state; ItemGroup recursively clones children.
	 */
	virtual WorkComponent *clone() const = 0;	
	/**
	 * @brief Access the stored display name.
	 * @return Borrowed reference, valid while the component exists and is not renamed.
	 */
	const std::string &getName() const;		
	/**
	 * @brief Replace the display name and publish a change event.
	 * @param[in] name New stored name.
	 * @details Emits CHANGED when the name differs; an unchanged name is a no-op.
	 * Observer exceptions propagate after the name has been assigned.
	 */
	void setName(std::string name);	
};

#endif