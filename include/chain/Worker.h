#ifndef WORKER_H
#define WORKER_H

#include "../composite/Item.h"
#include <functional>
#include <string>
#include <vector>

class Item;

class Worker{
	private:
		std::string id;
		std::string name;
		std::string role;
		Worker *supervisor;
		std::vector<Worker *> subordinates;
		int workload;
		DecisionPolicy policy;
		std::vector<Item *> escalatedQueue;

		static const int MAX_CHAIN_DEPTH = 64;

	public:
		Worker(std::string id, std::string name, std::string role, Worker *supervisor = NULL);
		bool review(Item *item, int depth);
		void handleEscalation(Item *item);
		int getWorkload();
		void addWorkload(int delta);

		void setSupervisor(Worker *s);
		void addSubordinate(Worker *w);
		void setDecisionPolicy(DecisionPolicy p);

		Worker *getSupervisor() const;
		const std::string &getId() const;
		const std::string &getName() const;
		const std::string &getRole() const;
		const std::vector<Item *> &getEscalatedQueue() const;

		typedef std::function<bool(Item *)> DecisionPolicy;
};

#endif