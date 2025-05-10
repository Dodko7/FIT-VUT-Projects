/**
 * @file test_outputs.cpp
 * @brief Unit tests for adding and removing outputs in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"

TEST_CASE("Output management", "[outputs]") {
    FSM fsm;

    SECTION("Add valid output") {
        REQUIRE_NOTHROW(fsm.addVariable("out", "initial_value"));
        REQUIRE(fsm.getVariables().find("out") != fsm.getVariables().end());
        REQUIRE(fsm.getVariables().at("out") == "initial_value");
    }

    SECTION("Add output with empty name") {
        REQUIRE_THROWS_AS(fsm.addVariable("", "value"), std::invalid_argument);
    }

    SECTION("Add output with long name") {
        std::string longName(21, 'a');
        REQUIRE_THROWS_AS(fsm.addVariable(longName, "value"), std::invalid_argument);
    }

    SECTION("Add duplicate output") {
        fsm.addVariable("out", "value");
        REQUIRE_THROWS_AS(fsm.addVariable("out", "value"), std::invalid_argument);
    }

    SECTION("Remove existing output") {
        fsm.addVariable("out", "value");
        REQUIRE_NOTHROW(fsm.removeVariable("out"));
        REQUIRE(fsm.getVariables().find("out") == fsm.getVariables().end());
    }

    SECTION("Remove non-existing output") {
        REQUIRE_NOTHROW(fsm.removeVariable("out")); // Should not throw
    }
}