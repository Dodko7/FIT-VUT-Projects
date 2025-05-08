/**
 * @file test_transitions.cpp
 * @brief Unit tests for adding and removing transitions in the FSM.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include "../src/core/fsmErrors.hpp"

TEST_CASE("Transition management", "[transitions]") {
    FSM fsm;
    fsm.addState("S0", "Initial state", false);
    fsm.addState("S1", "Next state", false);
    std::string s0 = "S0", s1 = "S1";

    SECTION("Add valid transition") {
        REQUIRE_NOTHROW(fsm.addTransition(s0, s1, 'a'));
        auto state = fsm.getStatePtrByName(s0);
        REQUIRE(state->getDependencies().size() == 1);
        REQUIRE(state->getDependencies()[0]->getExpectedInput() == 'a');
        REQUIRE(state->getNextStates().size() == 1);
        REQUIRE(state->getNextStates()[0]->getName() == "S1");
    }

    SECTION("Add transition with non-existing state") {
        std::string s2 = "S2";
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s2, 'a'), InvalidStateException);
        REQUIRE_THROWS_AS(fsm.addTransition(s2, s1, 'a'), InvalidStateException);
    }

    SECTION("Add transition with null input") {
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s1, '\0'), InvalidInputException);
    }

    SECTION("Add duplicate transition (determinism violation)") {
        fsm.addTransition(s0, s1, 'a');
        REQUIRE_THROWS_AS(fsm.addTransition(s0, s1, 'a'), DeterminismViolationException);
    }

    SECTION("Remove existing transition") {
        fsm.addTransition(s0, s1, 'a');
        REQUIRE_NOTHROW(fsm.removeTransition(s0, s1, 'a'));
        auto state = fsm.getStatePtrByName(s0);
        REQUIRE(state->getDependencies().empty());
        REQUIRE(state->getNextStates().empty());
    }

    SECTION("Remove non-existing transition") {
        REQUIRE_NOTHROW(fsm.removeTransition(s0, s1, 'a')); // Should not throw
    }

    SECTION("Remove transition with non-existing state") {
        std::string s2 = "S2";
        REQUIRE_THROWS_AS(fsm.removeTransition(s0, s2, 'a'), InvalidStateException);
        REQUIRE_THROWS_AS(fsm.removeTransition(s2, s1, 'a'), InvalidStateException);
    }
}