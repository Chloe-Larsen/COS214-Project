#include <string>
#include <iostream>

#include "catch.h"
#include "composite/Item.h"
#include "state/State.h"
#include "state/Available.h"
#include "observer/Dependency.h"

TEST_CASE("Basic item operations", "[item]")
{
    SECTION("Item progression works correctly")
    {
        Item item;

        REQUIRE(item.getState() != nullptr);
        REQUIRE(item.getState()->getName() == "AVAILABLE");

        item.progress();

        REQUIRE(item.getState()->getName() == "IN_PROGRESS");

        item.progress();

        REQUIRE(item.getState()->getName() == "COMPLETED");
    }

    SECTION("Item cancellation works correctly")
    {
        // can be cancelled if available
        Item item0;
        item0.cancel();
        REQUIRE(item0.getState()->getName() == "CANCELLED");

        // can be cancelled if in progress
        Item item1;
        item1.progress(); // progress to IN_PROGRESS
        item1.cancel();
        REQUIRE(item1.getState()->getName() == "CANCELLED");

        // cannot be cancelled if completed
        Item item2;
        item2.progress(); // progress to IN_PROGRESS`
        REQUIRE(item2.getState()->getName() == "IN_PROGRESS");
        item2.progress(); // progress to COMPLETED
        REQUIRE(item2.getState()->getName() == "COMPLETED");
        item2.cancel();
        REQUIRE(item2.getState()->getName() == "COMPLETED");
    }

    SECTION("Item dependencies work correctly")
    {
        // items to depend on
        Item item0;
        REQUIRE(item0.getDependencies() == 0);
        Item item1;

        // item that depends on other items
        Item dependentItem;
        item0.attach(new Dependency(&dependentItem));
        REQUIRE(dependentItem.getDependencies() == 1);
        item1.attach(new Dependency(&dependentItem));
        REQUIRE(dependentItem.getDependencies() == 2);

        // mark items as complete
        item1.notify(new Event("APPROVED", &item1));
        REQUIRE(dependentItem.getDependencies() == 1);
        item0.notify(new Event("APPROVED", &item0));
        REQUIRE(dependentItem.getDependencies() == 0);
    }
}