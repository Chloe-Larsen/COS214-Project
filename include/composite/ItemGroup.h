#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include "WorkComponent.h"
#include <vector>

class ItemGroup : public WorkComponent {
private:
	std::vector<WorkComponent*> children;
public:
	ItemGroup(std::string id, std::string name, std::string status);
	ItemGroup(ItemGroup& other);
	~ItemGroup();
	void progress();
	void cancel();
	void requestReview();
	void add(WorkComponent* child);
	void remove(WorkComponent* child);
	ItemGroup* clone();
	void progress() override;
	void cancel() override;
	void requestReview() override;
	Iterator* createIterator(std::string type) override;
	int getChildCount() override;	
};

#endif
