/**
 * @file test_inputs.cpp
 * @brief Unit tests for adding and removing expected inputs in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"

TEST_CASE("Input management", "[inputs]") {
    FSM fsm;

    SECTION("Add valid expected input") {
        REQUIRE_NOTHROW(fsm.addExpectedInput("in"));
        auto inputs = fsm.getExpectedInputs();
        REQUIRE(inputs.find("in") != inputs.end());
    }

    SECTION("Add input with empty name") {
        REQUIRE_THROWS_AS(fsm.addExpectedInput(""), std::invalid_argument);
    }

    SECTION("Add duplicate input") {
        fsm.addExpectedInput("in");
        REQUIRE_THROWS_AS(fsm.addExpectedInput("in"), std::invalid_argument);
    }

    SECTION("Remove existing input") {
        fsm.addExpectedInput("in");
        REQUIRE_NOTHROW(fsm.removeExpectedInput("in"));
        auto inputs = fsm.getExpectedInputs();
        REQUIRE(inputs.find("in") == inputs.end());
    }

    SECTION("Remove non-existing input") {
        REQUIRE_NOTHROW(fsm.removeExpectedInput("in")); // Should not throw
    }
    
    SECTION("Check valid input") {
        fsm.addExpectedInput("in");
        fsm.setInput("in");
        REQUIRE(fsm.checkValidInput() == true);
    }

    SECTION("Check invalid input") {
        fsm.setInput("invalid");
        REQUIRE(fsm.checkValidInput() == false);
    }
}