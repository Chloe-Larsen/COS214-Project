#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include "WorkComponent.h"
#include <vector>

class ItemGroup : public WorkComponent {
	private:
		std::vector<WorkComponent*> children;
		ItemGroup& operator=(const ItemGroup&);
		
	public:
		ItemGroup(std::string name);
		ItemGroup(const ItemGroup& other);
		~ItemGroup();
		void progress() override;
		void cancel() override;
		void requestReview() override;
		void add(WorkComponent* child);
		void remove(WorkComponent* child);
		ItemGroup* clone() const override;
		Iterator* createIterator(std::string type) override;
		int getChildCount() override;
		WorkComponent* getChild(int index) const;	
};

#endif