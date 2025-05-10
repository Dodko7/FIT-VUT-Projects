/**
 * @file test_inputs.cpp
 * @brief Unit tests for adding and removing character inputs in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include "../src/core/fsmErrors.hpp" // Added for InvalidArgumentException

TEST_CASE("Input management", "[inputs]") {
    FSM fsm;

    SECTION("Add valid input") {
        REQUIRE_NOTHROW(fsm.addExpectedInput('a'));
        auto inputs = fsm.getExpectedInputs();
        REQUIRE(inputs.find('a') != inputs.end());
    }

    SECTION("Add null character input") {
        REQUIRE_THROWS_AS(fsm.addExpectedInput('\0'), InvalidArgumentException);
    }

    SECTION("Add duplicate input") {
        fsm.addExpectedInput('a');
        REQUIRE_THROWS_AS(fsm.addExpectedInput('a'), InvalidArgumentException);
    }

    SECTION("Remove existing input") {
        fsm.addExpectedInput('a');
        REQUIRE_NOTHROW(fsm.removeExpectedInput('a'));
        auto inputs = fsm.getExpectedInputs();
        REQUIRE(inputs.find('a') == inputs.end());
    }

    SECTION("Remove non-existing input") {
        REQUIRE_NOTHROW(fsm.removeExpectedInput('x')); // Should not throw
    }
    
    SECTION("Check valid input") {
        fsm.addExpectedInput('a');
        fsm.setInput("a");
        REQUIRE(fsm.checkValidInput() == true);
    }

    SECTION("Check invalid input") {
        fsm.addExpectedInput('a');
        fsm.setInput("b");
        REQUIRE(fsm.checkValidInput() == false);
    }
}