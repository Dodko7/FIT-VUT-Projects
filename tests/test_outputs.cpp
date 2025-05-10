/**
 * @file test_outputs.cpp
 * @brief Unit tests for adding and removing outputs in the FSM.
 */

#include "catch.hpp"
#include "fsm.hpp"

TEST_CASE("Output management", "[outputs]") {
    FSM fsm;

    SECTION("Add valid output") {
        REQUIRE_NOTHROW(fsm.addOutput("out", "initial_value"));
        REQUIRE(fsm.getOutputs().find("out") != fsm.getOutputs().end());
        REQUIRE(fsm.getOutputs().at("out") == "initial_value");
    }

    SECTION("Add output with empty name") {
        REQUIRE_THROWS_AS(fsm.addOutput("", "value"), std::invalid_argument);
    }

    SECTION("Add output with long name") {
        std::string longName(21, 'a');
        REQUIRE_THROWS_AS(fsm.addOutput(longName, "value"), std::invalid_argument);
    }

    SECTION("Add duplicate output") {
        fsm.addOutput("out", "value");
        REQUIRE_THROWS_AS(fsm.addOutput("out", "value"), std::invalid_argument);
    }

    SECTION("Remove existing output") {
        fsm.addOutput("out", "value");
        REQUIRE_NOTHROW(fsm.removeOutput("out"));
        REQUIRE(fsm.getOutputs().find("out") == fsm.getOutputs().end());
    }

    SECTION("Remove non-existing output") {
        REQUIRE_NOTHROW(fsm.removeOutput("out")); // Should not throw
    }
}