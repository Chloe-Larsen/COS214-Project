#include <string>
#include <iostream>

#include "catch.h"
#include "WorkflowManager.h"
#include "composite/Item.h"
#include "composite/ItemGroup.h"
#include "strategy/WorkloadBased.h"

TEST_CASE("Workflow manager methods", "[workflow manager]")
{
    SECTION("Progresses every item in item tree")
    {
        ItemGroup *root = new ItemGroup();
        WorkflowManager manager(root);

        ItemGroup *secondary = new ItemGroup();
        root->add(secondary);

        Item *item0 = new Item();
        root->add(item0);

        Item *item1 = new Item();
        secondary->add(item1);

        Item *item2 = new Item();
        secondary->add(item2);

        REQUIRE(item0->getState()->getName() == "AVAILABLE");
        REQUIRE(item1->getState()->getName() == "AVAILABLE");
        REQUIRE(item2->getState()->getName() == "AVAILABLE");

        manager.progress();

        REQUIRE(item0->getState()->getName() == "IN_PROGRESS");
        REQUIRE(item1->getState()->getName() == "IN_PROGRESS");
        REQUIRE(item2->getState()->getName() == "IN_PROGRESS");
    }

    SECTION("Assigns work correctly")
    {
        ItemGroup *root = new ItemGroup();
        WorkflowManager manager(root);

        // create items
        Item *item0 = new Item();
        root->add(item0);
        Item *item1 = new Item();
        root->add(item1);

        // create workers
        Worker *worker0 = new Worker("w0", "Mr 0", "Manager");
        manager.addWorker(worker0);
        REQUIRE(worker0->getWorkload() == 0);
        Worker *worker1 = new Worker("w1", "Mr 1", "Employee");
        manager.addWorker(worker1);
        REQUIRE(worker1->getWorkload() == 0);

        // assign work to worker
        item0->assignWorker(worker0);
        REQUIRE(worker0->getWorkload() == 1);

        manager.setAssignmentStrategy(new WorkloadBased());
        manager.assignWork(item1);
        REQUIRE(worker1->getWorkload() == 1);
    }

    SECTION("Executed commands are stored in history")
    {
        ItemGroup *root = new ItemGroup();
        WorkflowManager manager(root);

        Item *item0 = new Item();
        root->add(item0);

        REQUIRE(manager.getHistory().size() == 0);
        manager.progress();
        REQUIRE(manager.getHistory().size() == 1);
    }
}