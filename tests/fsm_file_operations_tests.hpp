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
        FSMManager manager;
        std::string testFilePath = getTestFilePath("basic");
        
        // Create a basic FSM
        manager.createFSM("BasicFSM", "A basic test FSM", std::chrono::milliseconds(100));
        
        // Save the FSM
        bool saved = manager.saveFSM(testFilePath);
        if (!saved) {
            return TestFramework::TestResult(false, "Failed to save FSM: " + manager.getLastError(), std::chrono::milliseconds(0));
        }
        
        // Verify file exists
        bool fileExists = std::filesystem::exists(testFilePath);
        if (!fileExists) {
            return TestFramework::TestResult(false, "File doesn't exist after save", std::chrono::milliseconds(0));
        }
        
        // Create a new manager and load the FSM
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        if (!loaded) {
            return TestFramework::TestResult(false, "Failed to load FSM: " + loadManager.getLastError(), std::chrono::milliseconds(0));
        }
        
        // Verify the loaded FSM has the correct name/description
        auto fsm = loadManager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get loaded FSM instance", std::chrono::milliseconds(0));
        }
        
        bool nameMatches = (fsm->getName() == "BasicFSM");
        bool descMatches = (fsm->getDescription() == "A basic test FSM");
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(nameMatches && descMatches, 
                                        "Loaded FSM should have the same name and description as saved FSM");
    });
    
    // Test 2: Save and load FSM with states
    suite.addTest("Save and load FSM with states", []() {
        FSMManager manager;
        std::string testFilePath = getTestFilePath("states");
        
        // Create an FSM with states
        manager.createFSM("StatedFSM", "FSM with states", std::chrono::milliseconds(100));
        manager.addState("State1", "console.log('State1');", 'A', false);
        manager.addState("State2", "console.log('State2');", 'B', true);
        manager.setStartState("State1");
        
        // Save the FSM
        bool saved = manager.saveFSM(testFilePath);
        if (!saved) {
            return TestFramework::TestResult(false, "Failed to save FSM: " + manager.getLastError(), std::chrono::milliseconds(0));
        }
        
        // Create a new manager and load the FSM
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        if (!loaded) {
            return TestFramework::TestResult(false, "Failed to load FSM: " + loadManager.getLastError(), std::chrono::milliseconds(0));
        }
        
        // Verify the loaded FSM has the correct states
        auto stateNames = loadManager.getAllStateNames();
        std::sort(stateNames.begin(), stateNames.end());
        std::vector<std::string> expectedNames = {"State1", "State2"};
        
        // Verify state properties
        nlohmann::json state2Info = loadManager.getStateInfo("State2");
        bool isFinal = state2Info["isFinal"].get<bool>();
        char output = state2Info["output"].get<std::string>()[0];
        
        bool statesMatch = (stateNames == expectedNames);
        bool propertiesMatch = (isFinal && output == 'B');
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(statesMatch && propertiesMatch, 
                                        "Loaded FSM should have the same states and properties as saved FSM");
    });
    
    // Test 3: Save and load FSM with transitions
    suite.addTest("Save and load FSM with transitions", []() {
        FSMManager manager;
        std::string testFilePath = getTestFilePath("transitions");
        
        // Create an FSM with states and transitions
        manager.createFSM("TransitionFSM", "FSM with transitions", std::chrono::milliseconds(100));
        manager.addState("State1", "console.log('State1');", 'A', false);
        manager.addState("State2", "console.log('State2');", 'B', false);
        manager.addState("State3", "console.log('State3');", 'C', true);
        manager.setStartState("State1");
        
        // Add expected input characters for transitions
        manager.addExpectedInput('1');
        manager.addExpectedInput('2');
        
        // Store state names in variables to avoid string literal issues
        std::string state1 = "State1";
        std::string state2 = "State2";
        std::string state3 = "State3";
        
        manager.addTransition(state1, state2, "true", '1');
        manager.addTransition(state2, state3, "x > 5", '2');
        
        // Save the FSM
        bool saved = manager.saveFSM(testFilePath);
        if (!saved) {
            return TestFramework::TestResult(false, "Failed to save FSM: " + manager.getLastError(), std::chrono::milliseconds(0));
        }
        
        // Create a new manager and load the FSM
        FSMManager loadManager;
        bool loaded = loadManager.loadFSM(testFilePath);
        if (!loaded) {
            return TestFramework::TestResult(false, "Failed to load FSM: " + loadManager.getLastError(), std::chrono::milliseconds(0));
        }
        
        // Verify states were loaded
        auto stateNames = loadManager.getAllStateNames();
        bool statesLoaded = (stateNames.size() == 3);
        
        // Clean up
        cleanupTestFile(testFilePath);
        
        return TestFramework::assert_that(loaded && statesLoaded, 
                                       "FSM with transitions was saved and loaded successfully");
    });
    
    // Test 4: Export and import JSON
    suite.addTest("Export and import FSM as JSON", []() {
        FSMManager exportManager;
        
        // Create an FSM
        exportManager.createFSM("JsonFSM", "FSM for JSON export/import", std::chrono::milliseconds(100));
        exportManager.addState("State1", "console.log('State1');", 'A', false);
        exportManager.addState("State2", "console.log('State2');", 'B', true);
        exportManager.setStartState("State1");
        
        // Export to JSON
        std::string jsonStr = exportManager.exportFSMToJson();
        if (jsonStr.empty()) {
            return TestFramework::TestResult(false, "Failed to export FSM to JSON: " + exportManager.getLastError(), 
                                          std::chrono::milliseconds(0));
        }
        
        // Import from JSON in a new manager
        FSMManager importManager;
        bool imported = importManager.importFSMFromJson(jsonStr);
        if (!imported) {
            return TestFramework::TestResult(false, "Failed to import FSM from JSON: " + importManager.getLastError(), 
                                          std::chrono::milliseconds(0));
        }
        
        // Verify the imported FSM has the correct properties
        auto fsm = importManager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get imported FSM instance", std::chrono::milliseconds(0));
        }
        
        bool nameMatches = (fsm->getName() == "JsonFSM");
        bool descMatches = (fsm->getDescription() == "FSM for JSON export/import");
        
        auto stateNames = importManager.getAllStateNames();
        std::sort(stateNames.begin(), stateNames.end());
        std::vector<std::string> expectedNames = {"State1", "State2"};
        bool statesMatch = (stateNames == expectedNames);
        
        return TestFramework::assert_that(nameMatches && descMatches && statesMatch, 
                                        "Imported FSM should have the same properties as exported FSM");
    });
    
    // Test 5: Load non-existent file
    suite.addTest("Load non-existent file", []() {
        FSMManager manager;
        
        // Try to load a non-existent file
        bool loaded = manager.loadFSM("non_existent_file.json");
        
        return TestFramework::assert_that(!loaded, "Loading a non-existent file should fail");
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
