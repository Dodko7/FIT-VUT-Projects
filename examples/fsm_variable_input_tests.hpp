#ifndef FSM_VARIABLE_INPUT_TESTS_HPP
#define FSM_VARIABLE_INPUT_TESTS_HPP

#include "test_framework.hpp"
#include "fsmInterface.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

/**
 * @file fsm_variable_input_tests.hpp
 * @brief Tests for FSM variable and input handling
 * 
 * This file contains tests related to adding, removing, and managing
 * variables and input/output handling in the FSM.
 */

namespace FSMVariableInputTests {

/**
 * @brief Helper function to create a standard test FSM
 * @param manager Reference to the FSM manager to use
 * @return True if successful, false otherwise
 */
bool setupStandardTestFSM(FSMManager& manager) {
    if (!manager.createFSM("VarInputTestFSM", "FSM for testing variables and inputs", std::chrono::milliseconds(100))) {
        return false;
    }
    
    if (!manager.addState("State1", "console.log('State1: ' + x);", 'A', false)) {
        return false;
    }
    
    if (!manager.addState("State2", "console.log('State2: ' + y);", 'B', false)) {
        return false;
    }
    
    if (!manager.addState("Final", "console.log('Final state');", 'C', true)) {
        return false;
    }
    
    if (!manager.setStartState("State1")) {
        return false;
    }
    
    return true;
}

/**
 * @brief Creates and returns a test suite for FSM variable and input handling tests
 * @return A TestSuite object configured with variable and input management tests
 */
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("FSM Variable and Input Tests", "Tests for variable and input management operations in the FSM");
    
    // Setup function for the suite
    suite.setSuiteSetup([]() {
        std::cout << "Setting up FSM variable and input test suite..." << std::endl;
        return true; // Setup successful
    });
    
    // Teardown function for the suite
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down FSM variable and input test suite..." << std::endl;
        return true; // Teardown successful
    });
    
    // Test 1: Add a variable
    suite.addTest("Add a variable", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addVariable("counter", "0", true);
        
        auto variables = manager.getVariables();
        bool exists = variables.find("counter") != variables.end();
        bool correctValue = exists && variables["counter"] == "0";
        
        return TestFramework::assert_that(added && exists && correctValue, 
                                        "Variable should be added with correct value");
    });
    
    // Test 2: Add a variable with non-numeric value
    suite.addTest("Add a variable with non-numeric value", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addVariable("text", "Hello World", true);
        
        auto variables = manager.getVariables();
        bool exists = variables.find("text") != variables.end();
        bool correctValue = exists && variables["text"] == "Hello World";
        
        return TestFramework::assert_that(added && exists && correctValue, 
                                        "Variable with non-numeric value should be added correctly");
    });
    
    // Test 3: Add a variable with empty name
    suite.addTest("Add a variable with empty name", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addVariable("", "value", true);
        
        return TestFramework::assert_that(!added, "Adding variable with empty name should fail");
    });
    
    // Test 4: Update existing variable
    suite.addTest("Update existing variable", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addVariable("counter", "0", true);
        bool updated = manager.addVariable("counter", "5", true);
        
        auto variables = manager.getVariables();
        bool correctValue = variables["counter"] == "5";
        
        return TestFramework::assert_that(updated && correctValue, 
                                        "Variable should be updated when overwrite is true");
    });
    
    // Test 5: Try to update existing variable with overwrite=false
    suite.addTest("Try to update existing variable with overwrite=false", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addVariable("counter", "0", true);
        bool updated = manager.addVariable("counter", "5", false);
        
        auto variables = manager.getVariables();
        bool valueUnchanged = variables["counter"] == "0";
        
        return TestFramework::assert_that(!updated && valueUnchanged, 
                                        "Variable should not be updated when overwrite is false");
    });
    
    // Test 6: Remove a variable
    suite.addTest("Remove a variable", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addVariable("toRemove", "value", true);
        
        auto beforeRemoval = manager.getVariables();
        bool existsBefore = beforeRemoval.find("toRemove") != beforeRemoval.end();
        
        bool removed = manager.removeVariable("toRemove");
        
        auto afterRemoval = manager.getVariables();
        bool existsAfter = afterRemoval.find("toRemove") != afterRemoval.end();
        
        return TestFramework::assert_that(existsBefore && removed && !existsAfter, 
                                        "Variable should be removed successfully");
    });
    
    // Test 7: Remove a non-existent variable
    suite.addTest("Remove a non-existent variable", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool removed = manager.removeVariable("nonExistent");
        
        return TestFramework::assert_that(!removed, "Removing non-existent variable should fail");
    });
    
    // Test 8: Get all variables
    suite.addTest("Get all variables", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addVariable("var1", "value1", true);
        manager.addVariable("var2", "value2", true);
        manager.addVariable("var3", "value3", true);
        
        auto variables = manager.getVariables();
        bool allExist = variables.find("var1") != variables.end() &&
                        variables.find("var2") != variables.end() &&
                        variables.find("var3") != variables.end();
        bool correctSize = variables.size() == 3;
        
        return TestFramework::assert_that(allExist && correctSize, 
                                        "Should return all variables correctly");
    });
    
    // Test 9: Add expected input character
    suite.addTest("Add expected input character", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addExpectedInput('A');
        
        auto inputs = manager.getExpectedInputs();
        bool exists = std::find(inputs.begin(), inputs.end(), 'A') != inputs.end();
        
        return TestFramework::assert_that(added && exists, 
                                        "Expected input character should be added successfully");
    });
    
    // Test 10: Add duplicate expected input character
    suite.addTest("Add duplicate expected input character", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addExpectedInput('A');
        bool addedDuplicate = manager.addExpectedInput('A');
        
        auto inputs = manager.getExpectedInputs();
        int count = std::count(inputs.begin(), inputs.end(), 'A');
        
        // Behavior might vary depending on implementation
        // Here we assume duplicates are rejected
        return TestFramework::assert_that(!addedDuplicate && count == 1, 
                                        "Duplicate expected input should not be added");
    });
    
    // Test 11: Remove expected input character
    suite.addTest("Remove expected input character", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addExpectedInput('A');
        
        auto beforeRemoval = manager.getExpectedInputs();
        bool existsBefore = std::find(beforeRemoval.begin(), beforeRemoval.end(), 'A') != beforeRemoval.end();
        
        bool removed = manager.removeExpectedInput('A');
        
        auto afterRemoval = manager.getExpectedInputs();
        bool existsAfter = std::find(afterRemoval.begin(), afterRemoval.end(), 'A') != afterRemoval.end();
        
        return TestFramework::assert_that(existsBefore && removed && !existsAfter, 
                                        "Expected input character should be removed successfully");
    });
    
    // Test 12: Remove non-existent expected input character
    suite.addTest("Remove non-existent expected input character", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool removed = manager.removeExpectedInput('Z');
        
        return TestFramework::assert_that(!removed, "Removing non-existent expected input should fail");
    });
    
    // Test 13: Get all expected input characters
    suite.addTest("Get all expected input characters", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addExpectedInput('A');
        manager.addExpectedInput('B');
        manager.addExpectedInput('C');
        
        auto inputs = manager.getExpectedInputs();
        bool allExist = std::find(inputs.begin(), inputs.end(), 'A') != inputs.end() &&
                        std::find(inputs.begin(), inputs.end(), 'B') != inputs.end() &&
                        std::find(inputs.begin(), inputs.end(), 'C') != inputs.end();
        bool correctSize = inputs.size() == 3;
        
        return TestFramework::assert_that(allExist && correctSize, 
                                        "Should return all expected input characters correctly");
    });
    
    // Test 14: Set input string
    suite.addTest("Set input string", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        std::string inputStr = "ABCDEF";
        bool set = manager.setInput(inputStr);
        
        std::string retrievedInput = manager.getInput();
        
        return TestFramework::assert_that(set && retrievedInput == inputStr, 
                                        "Input string should be set and retrieved correctly");
    });
    
    // Test 15: Set empty input string
    suite.addTest("Set empty input string", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool set = manager.setInput("");
        
        std::string retrievedInput = manager.getInput();
        
        return TestFramework::assert_that(set && retrievedInput.empty(), 
                                        "Empty input string should be set successfully");
    });
    
    // Test 16: Set and get output string
    suite.addTest("Set and get output string", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        // Initial output should be empty
        std::string initialOutput = manager.getOutput();
        bool initialEmpty = initialOutput.empty();
        
        // Output is generally set during execution
        // Here we'll just check the initial state
        return TestFramework::assert_that(initialEmpty, 
                                        "Initial output string should be empty");
    });
    
    return suite;
}

} // namespace FSMVariableInputTests

#endif // FSM_VARIABLE_INPUT_TESTS_HPP
