/**
 * @file test_states.cpp
 * @brief Unit tests for adding and removing states in the FSM.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include "../src/core/fsmErrors.hpp"

TEST_CASE("State management", "[states]") {
    FSM fsm;

    SECTION("Add valid state") {
        REQUIRE_NOTHROW(fsm.addState("S0", "Initial state", false));
        REQUIRE(fsm.getStates().find("S0") != fsm.getStates().end());
        REQUIRE(fsm.getStates().at("S0")->getName() == "S0");
        REQUIRE(fsm.getStates().at("S0")->getAction() == "Initial state");
        REQUIRE_FALSE(fsm.getStates().at("S0")->getIsFinal());
    }

    SECTION("Add final state") {
        REQUIRE_NOTHROW(fsm.addState("S1", "Final state", true));
        REQUIRE(fsm.getFinalStates().find("S1") != fsm.getFinalStates().end());
    }

    SECTION("Add state with empty name") {
        REQUIRE_THROWS_AS(fsm.addState("", "Invalid state", false), std::invalid_argument);
    }

    SECTION("Add state with long name") {
        std::string longName(21, 'a');
        REQUIRE_THROWS_AS(fsm.addState(longName, "Long state", false), std::invalid_argument);
    }

    SECTION("Add duplicate state") {
        fsm.addState("S0", "Initial state", false);
        REQUIRE_THROWS_AS(fsm.addState("S0", "Duplicate state", false), InvalidStateException);
    }

    SECTION("Remove existing state") {
        fsm.addState("S0", "Initial state", false);
        REQUIRE_NOTHROW(fsm.removeState("S0"));
        REQUIRE(fsm.getStates().find("S0") == fsm.getStates().end());
    }

    SECTION("Remove non-existing state") {
        REQUIRE_NOTHROW(fsm.removeState("S0")); // Should not throw
    }

    SECTION("Remove state with transitions") {
        fsm.addState("S0", "Initial state", false);
        fsm.addState("S1", "Next state", false);
        std::string s0 = "S0", s1 = "S1";
        fsm.addTransition(s0, s1, "a", "", "", "");
        REQUIRE_NOTHROW(fsm.removeState("S0"));
        REQUIRE(fsm.getStates().find("S0") == fsm.getStates().end());
        REQUIRE(fsm.getStates().find("S1") == fsm.getStates().end()); // Unreachable state removed
    }

    SECTION("Remove final state") {
        fsm.addState("S0", "Final state", true);
        REQUIRE_NOTHROW(fsm.removeState("S0"));
        REQUIRE(fsm.getFinalStates().find("S0") == fsm.getFinalStates().end());
    }
}