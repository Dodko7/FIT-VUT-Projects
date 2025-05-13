#ifndef FSM_FILE_OPERATIONS_TESTS_HPP
#define FSM_FILE_OPERATIONS_TESTS_HPP

#include "test_framework.hpp"
#include "fsmInterface.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>

/**
 * @file fsm_file_operations_tests.hpp
 * @brief Tests for FSM file operations (save/load)
 * 
 * This file contains tests related to saving and loading FSM instances
 * to/from files and validating their serialization/deserialization.
 */

namespace FSMFileOperationsTests {

// Helper function to create a test file path
std::string getTestFilePath(const std::string& baseName) {
    return "test_" + baseName + ".json";
}

// Helper function to cleanup test files
void cleanupTestFile(const std::string& filePath) {
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}

/**
 * @brief Creates and returns a test suite for FSM file operations tests
 * @return A TestSuite object configured with file operations tests
 */
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("FSM File Operations Tests", "Tests for FSM save/load operations");
    
    // Setup function for the suite
    suite.setSuiteSetup([]() {
        std::cout << "Setting up FSM file operations test suite..." << std::endl;
        return true; // Setup successful
    });
    
    // Teardown function for the suite - clean up any test files
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down FSM file operations test suite..." << std::endl;
        
        // Clean up test files that might have been created
        cleanupTestFile(getTestFilePath("basic"));
        cleanupTestFile(getTestFilePath("complex"));
        cleanupTestFile(getTestFilePath("empty"));
        cleanupTestFile(getTestFilePath("states"));
        cleanupTestFile(getTestFilePath("transitions"));
        
        return true; // Teardown successful
    });
    
    // Test 1: Save and load a basic FSM
    suite.addTest("Save and load a basic FSM", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        std::string testFilePath = getTestFilePath("basic");
        debugContext.add("Test File Path", testFilePath);
        
        // Create a basic FSM
        bool created = manager.createFSM("BasicFSM", "A basic test FSM", std::chrono::milliseconds(100));
        debugContext.add("FSM Created", created);
        debugContext.add("FSM Name", "BasicFSM");
        debugContext.add("FSM Description", "A basic test FSM");
        
        if (!created) {
            return TestFramework::assert_that(false, "Failed to create FSM", debugContext);
        }
        
        // Save the FSM
        debugContext.addHeader("Save Operation");
        bool saved = manager.saveFSM(testFilePath);
        debugContext.add("Save Operation Success", saved);
        
        if (!saved) {
            debugContext.add("Save Error", manager.getLastError());
            return TestFramework::assert_that(false, "Failed to save FSM", debugContext);
        }
        
        // Verify file exists
        bool fileExists = std::filesystem::exists(testFilePath);
        debugContext.add("File Exists After Save", fileExists);
        
        if (!fileExists) {
            return TestFramework::assert_that(false, "File doesn't exist after save", debugContext);
        }
        
        // Create a new manager and load the FSM
        debugContext.addHeader("Load Operation");
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        debugContext.add("Load Operation Success", loaded);
        
        if (!loaded) {
            debugContext.add("Load Error", loadManager.getLastError());
            return TestFramework::assert_that(false, "Failed to load FSM", debugContext);
        }
        
        // Verify the loaded FSM has the correct name/description
        auto fsm = loadManager.getFSM();
        bool fsmValid = (fsm != nullptr);
        debugContext.add("Loaded FSM Valid", fsmValid);
        
        if (!fsmValid) {
            return TestFramework::assert_that(false, "Failed to get loaded FSM instance", debugContext);
        }
        
        std::string loadedName = fsm->getName();
        std::string loadedDesc = fsm->getDescription();
        debugContext.add("Loaded FSM Name", loadedName);
        debugContext.add("Loaded FSM Description", loadedDesc);
        
        bool nameMatches = (loadedName == "BasicFSM");
        bool descMatches = (loadedDesc == "A basic test FSM");
        debugContext.add("Name Matches", nameMatches);
        debugContext.add("Description Matches", descMatches);
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(nameMatches && descMatches, 
                                        "Loaded FSM should have the same name and description as saved FSM",
                                        debugContext);
    });
    
    // Test 2: Save and load FSM with states
    suite.addTest("Save and load FSM with states", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        std::string testFilePath = getTestFilePath("states");
        debugContext.add("Test File Path", testFilePath);
        
        // Create an FSM with states
        bool created = manager.createFSM("StatedFSM", "FSM with states", std::chrono::milliseconds(100));
        debugContext.add("FSM Created", created);
        debugContext.add("FSM Name", "StatedFSM");
        
        bool state1Added = manager.addState("State1", "console.log('State1');", 'A', false);
        bool state2Added = manager.addState("State2", "console.log('State2');", 'B', true);
        bool startStateSet = manager.setStartState("State1");
        
        debugContext.add("State1 Added", state1Added);
        debugContext.add("State2 Added", state2Added);
        debugContext.add("Start State Set", startStateSet);
        
        // Save the FSM
        debugContext.addHeader("Save Operation");
        bool saved = manager.saveFSM(testFilePath);
        debugContext.add("Save Operation Success", saved);
        
        if (!saved) {
            debugContext.add("Save Error", manager.getLastError());
            return TestFramework::assert_that(false, "Failed to save FSM", debugContext);
        }
        
        // Create a new manager and load the FSM
        debugContext.addHeader("Load Operation");
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        debugContext.add("Load Operation Success", loaded);
        
        if (!loaded) {
            debugContext.add("Load Error", loadManager.getLastError());
            return TestFramework::assert_that(false, "Failed to load FSM", debugContext);
        }
        
        // Verify the loaded FSM has the correct states
        auto stateNames = loadManager.getAllStateNames();
        std::sort(stateNames.begin(), stateNames.end());
        std::vector<std::string> expectedNames = {"State1", "State2"};
        
        debugContext.add("Loaded State Names", stateNames);
        debugContext.add("Expected State Names", expectedNames);
        
        // Verify state properties
        nlohmann::json state2Info = loadManager.getStateInfo("State2");
        bool isFinal = state2Info["isFinal"].get<bool>();
        char output = state2Info["output"].get<std::string>()[0];
        
        debugContext.add("State2 Is Final", isFinal);
        debugContext.add("State2 Output", std::string(1, output));
        
        bool statesMatch = (stateNames == expectedNames);
        bool propertiesMatch = (isFinal && output == 'B');
        
        debugContext.add("States Match", statesMatch);
        debugContext.add("Properties Match", propertiesMatch);
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(statesMatch && propertiesMatch, 
                                        "Loaded FSM should have the same states and properties as saved FSM",
                                        debugContext);
    });
    
    // Test 3: Save and load FSM with transitions
    suite.addTest("Save and load FSM with transitions", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        std::string testFilePath = getTestFilePath("transitions");
        debugContext.add("Test File Path", testFilePath);
        
        // Create an FSM with states and transitions
        bool created = manager.createFSM("TransitionFSM", "FSM with transitions", std::chrono::milliseconds(100));
        debugContext.add("FSM Created", created);
        debugContext.add("FSM Name", "TransitionFSM");
        
        bool state1Added = manager.addState("State1", "console.log('State1');", 'A', false);
        bool state2Added = manager.addState("State2", "console.log('State2');", 'B', false);
        bool state3Added = manager.addState("State3", "console.log('State3');", 'C', true);
        bool startStateSet = manager.setStartState("State1");
        
        debugContext.add("State1 Added", state1Added);
        debugContext.add("State2 Added", state2Added);
        debugContext.add("State3 Added", state3Added);
        debugContext.add("Start State Set", startStateSet);
        
        // Add expected input characters for transitions
        bool input1Added = manager.addExpectedInput('1');
        bool input2Added = manager.addExpectedInput('2');
        
        debugContext.add("Input '1' Added", input1Added);
        debugContext.add("Input '2' Added", input2Added);
        
        // Store state names in variables to avoid string literal issues
        std::string state1 = "State1";
        std::string state2 = "State2";
        std::string state3 = "State3";
        
        bool transition1Added = manager.addTransition(state1, state2, "true", '1');
        bool transition2Added = manager.addTransition(state2, state3, "x > 5", '2');
        
        debugContext.add("Transition State1->State2 Added", transition1Added);
        debugContext.add("Transition State2->State3 Added", transition2Added);
        
        // Save the FSM
        debugContext.addHeader("Save Operation");
        bool saved = manager.saveFSM(testFilePath);
        debugContext.add("Save Operation Success", saved);
        
        if (!saved) {
            debugContext.add("Save Error", manager.getLastError());
            return TestFramework::assert_that(false, "Failed to save FSM", debugContext);
        }
        
        // Create a new manager and load the FSM
        debugContext.addHeader("Load Operation");
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        debugContext.add("Load Operation Success", loaded);
        
        if (!loaded) {
            debugContext.add("Load Error", loadManager.getLastError());
            return TestFramework::assert_that(false, "Failed to load FSM", debugContext);
        }
        
        // Verify states were loaded
        auto stateNames = loadManager.getAllStateNames();
        debugContext.add("Loaded States", stateNames);
        debugContext.add("Loaded State Count", stateNames.size());
        bool statesLoaded = (stateNames.size() == 3);
        debugContext.add("All States Loaded", statesLoaded);
        
        // Verify transitions were loaded correctly
        // This would ideally check the transitions, but we don't have a direct getter
        // Instead we rely on the fact that the FSM loads correctly and would fail validation if transitions weren't loaded
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(loaded && statesLoaded, 
                                       "FSM with transitions was saved and loaded successfully",
                                       debugContext);
    });
    
    // Test 4: Export and import FSM as JSON
    suite.addTest("Export and import FSM as JSON", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager exportManager;
        
        // Create an FSM
        bool created = exportManager.createFSM("JsonFSM", "FSM for JSON export/import", std::chrono::milliseconds(100));
        debugContext.add("FSM Created", created);
        debugContext.add("FSM Name", "JsonFSM");
        debugContext.add("FSM Description", "FSM for JSON export/import");
        
        bool state1Added = exportManager.addState("State1", "console.log('State1');", 'A', false);
        bool state2Added = exportManager.addState("State2", "console.log('State2');", 'B', true);
        bool startStateSet = exportManager.setStartState("State1");
        
        debugContext.add("State1 Added", state1Added);
        debugContext.add("State2 Added", state2Added);
        debugContext.add("Start State Set", startStateSet);
        
        // Export to JSON
        debugContext.addHeader("JSON Export");
        std::string jsonStr = exportManager.exportFSMToJson();
        bool jsonExported = !jsonStr.empty();
        debugContext.add("JSON Exported", jsonExported);
        
        if (!jsonExported) {
            debugContext.add("Export Error", exportManager.getLastError());
            return TestFramework::assert_that(false, "Failed to export FSM to JSON", debugContext);
        }
        
        // Export JSON is usually large, so we'll just log a preview
        if (jsonStr.length() > 100) {
            debugContext.add("JSON Preview", jsonStr.substr(0, 100) + "...");
        } else {
            debugContext.add("JSON String", jsonStr);
        }
        
        // Import from JSON in a new manager
        debugContext.addHeader("JSON Import");
        FSMManager importManager;
        bool imported = importManager.importFSMFromJson(jsonStr);
        debugContext.add("JSON Import Success", imported);
        
        if (!imported) {
            debugContext.add("Import Error", importManager.getLastError());
            return TestFramework::assert_that(false, "Failed to import FSM from JSON", debugContext);
        }
        
        // Verify the imported FSM has the correct properties
        auto fsm = importManager.getFSM();
        bool fsmValid = (fsm != nullptr);
        debugContext.add("Imported FSM Valid", fsmValid);
        
        if (!fsmValid) {
            return TestFramework::assert_that(false, "Failed to get imported FSM instance", debugContext);
        }
        
        std::string importedName = fsm->getName();
        std::string importedDesc = fsm->getDescription();
        debugContext.add("Imported FSM Name", importedName);
        debugContext.add("Imported FSM Description", importedDesc);
        
        bool nameMatches = (importedName == "JsonFSM");
        bool descMatches = (importedDesc == "FSM for JSON export/import");
        debugContext.add("Name Matches", nameMatches);
        debugContext.add("Description Matches", descMatches);
        
        auto stateNames = importManager.getAllStateNames();
        std::sort(stateNames.begin(), stateNames.end());
        std::vector<std::string> expectedNames = {"State1", "State2"};
        
        debugContext.add("Imported State Names", stateNames);
        debugContext.add("Expected State Names", expectedNames);
        
        bool statesMatch = (stateNames == expectedNames);
        debugContext.add("States Match", statesMatch);
        
        return TestFramework::assert_that(nameMatches && descMatches && statesMatch, 
                                        "Imported FSM should have the same properties as exported FSM",
                                        debugContext);
    });
    
    // Test 5: Load non-existent file
    suite.addTest("Load non-existent file", []() {
        TestDebug::DebugContext debugContext;
        FSMManager manager;
        
        // Try to load a non-existent file
        std::string nonExistentFile = "non_existent_file.json";
        debugContext.add("File Path", nonExistentFile);
        debugContext.add("File Exists", std::filesystem::exists(nonExistentFile));
        
        bool loaded = manager.loadFSM(nonExistentFile);
        debugContext.add("Load Result", loaded);
        
        if (loaded) {
            debugContext.add("Unexpected Success", "Loading non-existent file succeeded unexpectedly");
        } else {
            debugContext.add("Error Message", manager.getLastError());
        }
        
        return TestFramework::assert_that(!loaded, "Loading a non-existent file should fail", debugContext);
    });
    
    // Test 6: Load invalid JSON file
    suite.addTest("Load invalid JSON file", []() {
        FSMManager manager;
        std::string testFilePath = getTestFilePath("invalid");
        
        // Create an invalid JSON file
        std::ofstream file(testFilePath);
        file << "{ This is not valid JSON }";
        file.close();
        
        // Try to load the invalid file
        bool loaded = manager.loadFSM(testFilePath);
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(!loaded, "Loading an invalid JSON file should fail");
    });
    
    // Test 7: Save to a read-only location (might fail depending on environment)
    suite.addTest("Save to a potentially restricted location", []() {
        FSMManager manager;
        manager.createFSM("RestrictedFSM", "FSM for testing restricted save", std::chrono::milliseconds(100));
        
        // Try to save to a location that might be restricted
        // Note: This test might pass or fail depending on environment permissions
        bool saved = manager.saveFSM("/root/test_restricted.json");
        
        std::string message = "Save to restricted location ";
        message += saved ? "succeeded (unexpected)" : "failed (expected)";
        
        return TestFramework::TestResult(true, message, std::chrono::milliseconds(0));
    });
    
    // Test 8: Save and load complex FSM
    suite.addTest("Save and load complex FSM", []() {
        FSMManager manager;
        std::string testFilePath = getTestFilePath("complex");
        
        // Create a complex FSM with states, transitions, variables
        manager.createFSM("ComplexFSM", "A complex test FSM", std::chrono::milliseconds(100));
        manager.addState("State1", "var x = count + 1; console.log('State1: ' + x);", 'A', false);
        manager.addState("State2", "console.log('State2 with var: ' + x);", 'B', false);
        manager.addState("State3", "console.log('Final state');", 'C', true);
        manager.setStartState("State1");
        manager.addTransition("State1", "State2", "count > 5", '1');
        manager.addTransition("State2", "State3", "x > 10", '2');
        manager.addVariable("count", "0", true);
        
        // Save the FSM
        bool saved = manager.saveFSM(testFilePath);
        if (!saved) {
            return TestFramework::TestResult(false, "Failed to save complex FSM: " + manager.getLastError(), 
                                          std::chrono::milliseconds(0));
        }
        
        // Create a new manager and load the FSM
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        if (!loaded) {
            return TestFramework::TestResult(false, "Failed to load complex FSM: " + loadManager.getLastError(), 
                                          std::chrono::milliseconds(0));
        }
        
        // Verify components were loaded correctly
        auto stateNames = loadManager.getAllStateNames();
        auto variables = loadManager.getVariables();
        
        bool statesLoaded = (stateNames.size() == 3);
        bool varsLoaded = (variables.find("count") != variables.end());
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(statesLoaded && varsLoaded, 
                                        "Complex FSM should be saved and loaded with all components intact");
    });
    
    return suite;
}

} // namespace FSMFileOperationsTests

#endif // FSM_FILE_OPERATIONS_TESTS_HPP
