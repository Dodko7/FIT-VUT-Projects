/**
 * @file test_json.cpp
 * @brief Unit tests for JSON serialization and deserialization.
 */

#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include <filesystem>
#include <fstream>

TEST_CASE("JSON serialization and deserialization", "[json]") {
    FSM fsm;
    fsm.setName("TOF5s");
    fsm.setDescription("Timer to off, simple version");
    
    // Add expected inputs as char
    fsm.addExpectedInput('i');
    fsm.addExpectedInput('s');
    fsm.addExpectedInput('r');
    
    // Add variable for output
    fsm.addVariable("out", "0");
    fsm.addVariable("timeout", "5000");
    
    // Add states
    fsm.addState("IDLE", "output('out', 0)", '\0', false);
    fsm.addState("ACTIVE", "output('out', 1)", '\0', false);
    fsm.addState("TIMING", "", '\0', false);
    
    fsm.setStartState("IDLE");
    
    // Add transitions
    fsm.addTransition("IDLE", "ACTIVE", "atoi(valueof('in')) == 1", 'i');
    fsm.addTransition("ACTIVE", "TIMING", "atoi(valueof('in')) == 0", 'i');
    fsm.addTransition("TIMING", "ACTIVE", "atoi(valueof('in')) == 1", 'i');
    fsm.addTransition("TIMING", "IDLE", "", 't');

    SECTION("Save and load JSON") {
        const std::string filename = "examples/test_fsm.json";
        try {
            fsm.saveToJson(filename);
            std::ifstream checkFile(filename);
            REQUIRE(checkFile.is_open() == true);
            checkFile.close();
        } catch (const std::runtime_error& e) {
            FAIL("Failed to save JSON: " << e.what());
        }

        FSM loadedFsm;
        loadedFsm.loadFromJson(filename);

        REQUIRE(loadedFsm.getName() == "TOF5s");
        REQUIRE(loadedFsm.getDescription() == "Timer to off, simple version");
        REQUIRE(loadedFsm.getExpectedInputs().find('i') != loadedFsm.getExpectedInputs().end());
        REQUIRE(loadedFsm.getVariables().at("out") == "0");
        REQUIRE(loadedFsm.getVariables().at("timeout") == "5000");
        REQUIRE(loadedFsm.getStates().size() == 3);
        REQUIRE(loadedFsm.getStartState()->getName() == "IDLE");

        // Verify transitions
        auto idleState = loadedFsm.getStatePtrByName("IDLE");
        REQUIRE(idleState->getDependencies().size() == 1);
        REQUIRE(idleState->getDependencies()[0]->getCondition() == "atoi(valueof('in')) == 1");
    }

    SECTION("Load invalid JSON") {
        const std::string filename = "invalid.json";
        std::ofstream file(filename);
        file << "invalid json";
        file.close();
        REQUIRE_THROWS_AS(fsm.loadFromJson(filename), std::runtime_error);
        std::filesystem::remove(filename);
    }
}