/**
 * @file ItemGroup.h
 * @brief Owning composite for nested work components.
 * @ingroup tividy_composite
 */

#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include "WorkComponent.h"
#include <vector>

/**
 * @brief Composite participant owning an ordered collection of components.
 * @details Children can be Item leaves or other ItemGroup objects. Lifecycle
 * operations are forwarded in insertion order, and getChildCount() aggregates
 * recursive leaves. Children are deleted with their owning group.
 * @ingroup tividy_composite
 */
class ItemGroup : public WorkComponent {
	private:
		/**
		 * @brief Owned direct children in insertion order.
		 * @details Ownership is transferred by add() and released by remove().
		 */
		std::vector<WorkComponent*> children;
		/**
		 * @brief Prevent assignment of child-owning groups.
		 * @details Intentionally private and undefined; use clone() for a structural copy.
		 */
		ItemGroup& operator=(const ItemGroup&);
		
	public:
		/**
		 * @brief Create an empty named group.
		 * @param[in] name Initial display name.
		 */
		ItemGroup(std::string name);
		/**
		 * @brief Deep-copy the group structure using each child's clone().
		 * @param[in] other Source group supplying its name, supported observers and children.
		 * @details Cleans up children already cloned if a later clone fails. Item child
		 * clones reset execution state according to Item's prototype semantics.
		 */
		ItemGroup(const ItemGroup& other);
		/**
		 * @brief Create an empty group with an empty builder-facing name.
		 */
		ItemGroup();
		/**
		 * @brief Delete every owned direct child and its descendants.
		 */
		~ItemGroup();
		/**
		 * @brief Forward progress() to each direct child in insertion order.
		 * @details Nested groups recurse through the same interface.
		 */
		void progress() override;
		/**
		 * @brief Forward cancel() to each direct child in insertion order.
		 * @details Propagates the first child exception.
		 */
		void cancel() override;
		/**
		 * @brief Forward requestReview() to each direct child in insertion order.
		 * @details Each leaf performs its own eligibility and review checks.
		 */
		void requestReview() override;
		/**
		 * @brief Append a child and take ownership on success.
		 * @param[in] child Non-null component whose ownership is being transferred.
		 * @throws std::string If child is null, already appears in this subtree, or would
		 * create an ancestor cycle.
		 * @pre The child is not owned by any other parent.
		 * @post On success this group owns child; if insertion fails, the caller retains ownership.
		 */
		void add(WorkComponent* child);
		/**
		 * @brief Detach a direct child without deleting it.
		 * @param[in] child Direct child to detach.
		 * @details On successful removal the caller becomes responsible for the child.
		 */
		void remove(WorkComponent* child);
		/**
		 * @brief Create a caller-owned structural prototype copy.
		 * @return Newly allocated group owning clones of all its descendants.
		 */
		ItemGroup* clone() const override;
		/**
		 * @brief Create an iterator rooted at this group.
		 * @param[in] type "Dependency", "dependency", "Priority" or "priority".
		 * @return Newly allocated iterator owned by the caller.
		 * @throws std::string If type is unknown.
		 * @pre This tree outlives the iterator and is not structurally changed during its traversal.
		 */
		Iterator* createIterator(std::string type) override;
		/**
		 * @brief Aggregate recursive leaf counts across all children.
		 * @return Number of Item leaves in this subtree; empty groups contribute zero.
		 */
		int getChildCount() override;
                /**
                 * @brief Count immediate children, including child groups.
                 * @return Number of entries that can be indexed using getChild().
                 */
        		int getDirectChildCount() const;
		/**
		 * @brief Access an immediate child by zero-based index.
		 * @param[in] index Position in the direct-child sequence.
		 * @return Borrowed child pointer, or null if index is negative or out of range.
		 * @details Ownership stays with the group until the child is detached.
		 */
		WorkComponent* getChild(int index) const;	
};

#endif