/**
 * @file test_fsm.cpp
 * @brief Unit tests for FSM runtime and advanced functionality.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include "../src/core/fsmErrors.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

TEST_CASE("FSM runtime and validation", "[fsm]") {
    FSM fsm;
    fsm.setName("TestFSM");
    fsm.setDescription("Test FSM for runtime");
    fsm.addState("S0", "Initial state action", '0', false);
    fsm.addState("S1", "Next state action", '1', false);
    fsm.addState("S2", "Final state action", '2', true);
    std::string s0 = "S0", s1 = "S1", s2 = "S2";
    fsm.addTransition(s0, s1, "a", "");  // Updated to remove output param
    fsm.addTransition(s1, s2, "b", "");  // Updated to remove output param
    fsm.setStartState("S0");

    SECTION("Run with valid input sequence") {
        fsm.run("ab");
        REQUIRE(fsm.getCurrentState()->getName() == "S2");
        REQUIRE(fsm.getCurrentMachineState() == machineState::STOPPED);
    }

    SECTION("Run with invalid input") {
        fsm.run("ac");
        REQUIRE(fsm.getCurrentState()->getName() == "S1"); // Corrected to S1
    }

    SECTION("Run without start state") {
        FSM fsm2;
        REQUIRE_THROWS_AS(fsm2.run("ab"), MooreMachineValidationException);
    }

    SECTION("Validate FSM with unreachable state") {
        fsm.addState("S3", "Unreachable action", '3', false);
        REQUIRE_NOTHROW(fsm.validateFSM()); // Should log unreachable state
    }

    SECTION("Prune unreachable states") {
        fsm.addState("S3", "Unreachable action", '3', false);
        REQUIRE(fsm.getStates().find("S3") != fsm.getStates().end()); // Ensure S3 exists
        fsm.pruneUnreachable();
        REQUIRE(fsm.getStates().find("S3") == fsm.getStates().end()); // S3 should be removed
    }

    SECTION("Debug generates valid DOT") {
        std::system("mkdir -p assets");
        fsm.debug();
        std::ifstream dotFile("assets/fsm_debug.dot");
        REQUIRE(dotFile.is_open()); // Check if file was created
        std::stringstream buffer;
        buffer << dotFile.rdbuf();
        std::string content = buffer.str();
        dotFile.close();
        std::cerr << "DOT file content:\n" << content << "\n"; // Debug output
        REQUIRE(!content.empty());
        REQUIRE(content.find("digraph FSM") != std::string::npos);
        REQUIRE(content.find("S0 -> S1 [label=\"a\"]") != std::string::npos);
        REQUIRE(content.find("S1 -> S2 [label=\"b\"]") != std::string::npos);
        REQUIRE(content.find("S0 [shape=doublecircle, color=green]") != std::string::npos);
        REQUIRE(content.find("S2 [shape=doublecircle, color=red]") != std::string::npos);
    }
}