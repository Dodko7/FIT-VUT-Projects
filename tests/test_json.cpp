#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include <filesystem>
#include <fstream>

TEST_CASE("JSON serialization and deserialization", "[json]") {
    FSM fsm;
    fsm.setName("TOF5s");
    fsm.setDescription("Timer to off, simple version");
    
    // Add expected input instead of addInput
    fsm.addExpectedInput("in");
    
    // Add variable for timeout
    fsm.addVariable("timeout", "5000");
    
    // Fix parameter order in addState (name, action, isFinal)
    fsm.addState("IDLE", "output('out', 0)", false);
    fsm.addState("ACTIVE", "output('out', 1)", false);
    fsm.addState("TIMING", "", false);
    
    fsm.setStartState("IDLE");
    
    // Add missing output parameter to addTransition calls
    fsm.addTransition("IDLE", "ACTIVE", "in", "atoi(valueof('in')) == 1", "", "");
    fsm.addTransition("ACTIVE", "TIMING", "in", "atoi(valueof('in')) == 0", "", "");
    fsm.addTransition("TIMING", "ACTIVE", "in", "atoi(valueof('in')) == 1", "", "");
    fsm.addTransition("TIMING", "IDLE", "", "", "timeout", "");

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
        
        // Use getExpectedInputs instead of getInputs
        auto expectedInputs = loadedFsm.getExpectedInputs();
        REQUIRE(expectedInputs.find("in") != expectedInputs.end());
        
        // Use getVariables instead of testing individual outputs
        REQUIRE(loadedFsm.getVariables().at("timeout") == "5000");
        REQUIRE(loadedFsm.getStates().size() == 3);
        REQUIRE(loadedFsm.getStartState()->getName() == "IDLE");

        // Testing transitions
        auto idleState = loadedFsm.getStatePtrByName(std::string("IDLE"));
        REQUIRE(idleState->getDependencies().size() == 1);
        REQUIRE(idleState->getDependencies()[0]->getEvent() == "in");
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