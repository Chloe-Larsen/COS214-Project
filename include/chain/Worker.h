#ifndef WORKER_H
#define WORKER_H

#include "../composite/Item.h"
#include <vector>
#include <string>

class Worker
{
private:
	std::string id;
	std::string name;
	std::string email;
	std::string role;
	Worker *supervisor;
	std::vector<Worker *> subordinates;

public:
	Worker(std::string id, std::string name, std::string email, std::string role, Worker *supervisor);

	bool review(Item *item, int depth);
	void handleEscalation(Item *item);
	int getWorkload();
	std::string getId();
	std::string getName();
	std::string getEmail();
	std::string getRole();
	Worker *getSupervisor();
};

#endif