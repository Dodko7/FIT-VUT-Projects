#include "catch.hpp"
#include "../src/core/fsm.hpp"
#include <filesystem>
#include <fstream> // Add this include for std::ofstream

TEST_CASE("JSON serialization and deserialization", "[json]") {
    FSM fsm;
    fsm.setName("TOF5s");
    fsm.setDescription("Timer to off, simple version");
    fsm.addInput("in", "0");
    fsm.addOutput("out", "0");
    fsm.addVariable("timeout", "5000");
    fsm.addState("IDLE", "IDLE action", '0', false);
    fsm.addState("ACTIVE", "ACTIVE action", '1', false);
    fsm.addState("TIMING", "TIMING action", '0', false);
    fsm.setStartState("IDLE");
    fsm.addTransition("IDLE", "ACTIVE", "in", "atoi(valueof('in')) == 1");
    fsm.addTransition("ACTIVE", "TIMING", "in", "atoi(valueof('in')) == 0");
    fsm.addTransition("TIMING", "ACTIVE", "in", "atoi(valueof('in')) == 1");
    fsm.addTransition("TIMING", "IDLE", "", "");

    SECTION("Save and load JSON") {
        const std::string filename = "examples/test_fsm.json";
        try {
            fsm.saveToJson(filename);
            std::ifstream checkFile(filename);
            REQUIRE(checkFile.is_open() == true); // Overi, či súbor existuje
            checkFile.close();
        } catch (const std::runtime_error& e) {
            FAIL("Failed to save JSON: " << e.what());
        }

        FSM loadedFsm;
        loadedFsm.loadFromJson(filename);

        REQUIRE(loadedFsm.getName() == "TOF5s");
        REQUIRE(loadedFsm.getDescription() == "Timer to off, simple version");
        REQUIRE(loadedFsm.getInputs().at("in") == "0");
        REQUIRE(loadedFsm.getOutputs().at("out") == "0");
        REQUIRE(loadedFsm.getVariables().at("timeout") == "5000");
        REQUIRE(loadedFsm.getStates().size() == 3);
        REQUIRE(loadedFsm.getStartState()->getName() == "IDLE");

        // Overenie prechodov
        auto idleState = loadedFsm.getStatePtrByName(std::string("IDLE"));
        REQUIRE(idleState->getDependencies().size() == 1);
        REQUIRE(idleState->getDependencies()[0]->getEvent() == "in");
        REQUIRE(idleState->getDependencies()[0]->getCondition() == "atoi(valueof('in')) == 1");

        // // Vyčistenie
        // std::filesystem::remove(filename);
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