/**
 * @file test_transitions.cpp
 * @brief Unit tests for transitions in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include "../src/core/fsmErrors.hpp"

TEST_CASE("Transition management", "[transitions]") {
    FSM fsm;
    fsm.addState("S0", "Initial state", '\0', false);
    fsm.addState("S1", "Next state", '\0', false);
    std::string s0 = "S0", s1 = "S1", s2 = "S2";

    SECTION("Add valid transition") {
        REQUIRE_NOTHROW(fsm.addTransition(s0, s1, "a", 'a'));
        auto state = fsm.getStates().at("S0");
        REQUIRE(state->getDependencies().size() == 1);
        REQUIRE(state->getNextStates().size() == 1);
        REQUIRE(state->getNextStates()[0]->getName() == "S1");
        REQUIRE(state->getDependencies()[0]->getCondition() == "a");
    }

    SECTION("Add transition with invalid state") {
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s2, "a", 'a'), InvalidStateException);
        REQUIRE_THROWS_AS(fsm.addTransition(s2, s1, "a", 'a'), InvalidStateException);
    }

    SECTION("Add transition with invalid input") {
        // Note: Empty condition is not allowed in this implementation
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s1, "", 'a'), InvalidInputException);
    }

    SECTION("Add duplicate transition") {
        fsm.addTransition(s0, s1, "a", 'a');
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s1, "a", 'a'), DeterminismViolationException);
    }

    SECTION("Remove existing transition") {
        fsm.addTransition(s0, s1, "a", 'a');
        REQUIRE_NOTHROW(fsm.removeTransition(s0, s1, 'a'));
        auto state = fsm.getStates().at("S0");
        REQUIRE(state->getDependencies().size() == 0);
        REQUIRE(state->getNextStates().size() == 0);
    }

    SECTION("Remove non-existing transition") {
        REQUIRE_NOTHROW(fsm.removeTransition(s0, s1, 'a'));
    }

    SECTION("Remove transition with invalid state") {
        REQUIRE_THROWS_AS(fsm.removeTransition(s0, s2, 'a'), InvalidStateException);
        REQUIRE_THROWS_AS(fsm.removeTransition(s2, s1, 'a'), InvalidStateException);
    }
}