/**
 * @file test_transitions.cpp
 * @brief Unit tests for transitions in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include "../src/core/fsmErrors.hpp"

TEST_CASE("Transition management", "[transitions]") {
    FSM fsm;
    fsm.addState("S0", "Initial state", "", false);  // Added action parameter
    fsm.addState("S1", "Next state", "", false);     // Added action parameter
    std::string s0 = "S0", s1 = "S1", s2 = "S2";

    SECTION("Add valid transition") {
        REQUIRE_NOTHROW(fsm.addTransition(s0, s1, "a", "", ""));  // Changed 'a' to "a" and added condition and timeout params
        auto state = fsm.getStates().at("S0");
        REQUIRE(state->getDependencies().size() == 1);
        REQUIRE(state->getNextStates().size() == 1);
        REQUIRE(state->getNextStates()[0]->getName() == "S1");
        REQUIRE(state->getDependencies()[0]->getEvent() == "a");  // Changed getExpectedInput() to getEvent()
    }

    SECTION("Add transition with invalid state") {
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s2, "a", "", ""), InvalidStateException);  // Updated parameters
        REQUIRE_THROWS_AS(fsm.addTransition(s2, s1, "a", "", ""), InvalidStateException);  // Updated parameters
    }

    SECTION("Add transition with invalid input") {
        // Note: The empty string is now a valid event, so using "" instead of '\0'
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s1, "", "", ""), InvalidInputException);  // Updated parameters
    }

    SECTION("Add duplicate transition") {
        fsm.addTransition(s0, s1, "a", "", "");  // Updated parameters
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s1, "a", "", ""), DeterminismViolationException);  // Updated parameters
    }

    SECTION("Remove existing transition") {
        fsm.addTransition(s0, s1, "a", "", "");  // Updated parameters
        REQUIRE_NOTHROW(fsm.removeTransition(s0, s1, 'a'));  // Changed back to character literal
        auto state = fsm.getStates().at("S0");
        REQUIRE(state->getDependencies().size() == 0);
        REQUIRE(state->getNextStates().size() == 0);
    }

    SECTION("Remove non-existing transition") {
        REQUIRE_NOTHROW(fsm.removeTransition(s0, s1, 'a'));  // Changed back to character literal
    }

    SECTION("Remove transition with invalid state") {
        REQUIRE_THROWS_AS(fsm.removeTransition(s0, s2, 'a'), InvalidStateException);  // Changed back to character literal
        REQUIRE_THROWS_AS(fsm.removeTransition(s2, s1, 'a'), InvalidStateException);  // Changed back to character literal
    }
}