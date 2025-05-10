/**
 * @file test_inputs.cpp
 * @brief Unit tests for adding and removing inputs in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"

TEST_CASE("Input management", "[inputs]") {
    FSM fsm;

    SECTION("Add valid input") {
        REQUIRE_NOTHROW(fsm.addInput("in", "initial_value"));
        REQUIRE(fsm.getInputs().find("in") != fsm.getInputs().end());
        REQUIRE(fsm.getInputs().at("in") == "initial_value"); // Overenie hodnoty
    }

    SECTION("Add input with empty name") {
        REQUIRE_THROWS_AS(fsm.addInput("", "value"), std::invalid_argument);
    }

    SECTION("Add input with long name") {
        std::string longName(21, 'a');
        REQUIRE_THROWS_AS(fsm.addInput(longName, "value"), std::invalid_argument);
    }

    SECTION("Add duplicate input") {
        fsm.addInput("in", "value");
        REQUIRE_THROWS_AS(fsm.addInput("in", "value"), std::invalid_argument);
    }

    SECTION("Remove existing input") {
        fsm.addInput("in", "value");
        REQUIRE_NOTHROW(fsm.removeInput("in"));
        REQUIRE(fsm.getInputs().find("in") == fsm.getInputs().end());
    }

    SECTION("Remove non-existing input") {
        REQUIRE_NOTHROW(fsm.removeInput("in")); // Should not throw
    }
}