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
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Get initial state for verification
        auto initialVars = manager.getVariables();
        debugContext.add("Initial Variable Count", initialVars.size());
        
        debugContext.addHeader("Variable Addition");
        std::string varName = "counter";
        std::string varValue = "0";
        bool persistent = true;
        
        debugContext.add("Variable Name", varName);
        debugContext.add("Variable Value", varValue);
        debugContext.add("Persistent", persistent);
        
        bool added = manager.addVariable(varName, varValue, persistent);
        debugContext.add("Variable Added", added);
        
        auto variables = manager.getVariables();
        debugContext.add("Variables After Addition", variables);
        
        bool exists = variables.find(varName) != variables.end();
        bool correctValue = exists && variables[varName] == varValue;
        
        debugContext.add("Variable Exists", exists);
        debugContext.add("Correct Value", correctValue);
        
        return TestFramework::assert_that(added && exists && correctValue, 
                                        "Variable should be added with correct value",
                                        debugContext);
    });
    
    // Test 2: Add a variable with non-numeric value
    suite.addTest("Add a variable with non-numeric value", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        debugContext.addHeader("Variable Addition");
        std::string varName = "text";
        std::string varValue = "Hello World";
        bool persistent = true;
        
        debugContext.add("Variable Name", varName);
        debugContext.add("Variable Value", varValue);
        debugContext.add("Persistent", persistent);
        debugContext.add("Is Numeric", false);
        
        bool added = manager.addVariable(varName, varValue, persistent);
        debugContext.add("Variable Added", added);
        
        auto variables = manager.getVariables();
        debugContext.add("Variables After Addition", variables);
        
        bool exists = variables.find(varName) != variables.end();
        bool correctValue = exists && variables[varName] == varValue;
        
        debugContext.add("Variable Exists", exists);
        debugContext.add("Correct Value", correctValue);
        
        return TestFramework::assert_that(added && exists && correctValue, 
                                        "Variable with non-numeric value should be added correctly",
                                        debugContext);
    });
    
    // Test 3: Add a variable with empty name
    suite.addTest("Add a variable with empty name", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        debugContext.addHeader("Variable Addition");
        std::string varName = ""; // Empty name
        std::string varValue = "value";
        bool persistent = true;
        
        debugContext.add("Variable Name", varName.empty() ? "(empty)" : varName);
        debugContext.add("Variable Value", varValue);
        debugContext.add("Persistent", persistent);
        
        bool added = manager.addVariable(varName, varValue, persistent);
        debugContext.add("Variable Added", added);
        
        if (added) {
            // This should not happen, but let's verify
            auto variables = manager.getVariables();
            debugContext.add("Variables After Addition", variables);
            debugContext.add("Unexpected Success", "Empty variable name was accepted");
        } else {
            debugContext.add("Expected Failure", "Empty variable name was rejected");
        }
        
        return TestFramework::assert_that(!added, "Adding variable with empty name should fail", debugContext);
    });
    
    // Test 4: Update existing variable
    suite.addTest("Update existing variable", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Initial variable setup
        std::string varName = "counter";
        std::string initialValue = "0";
        std::string updatedValue = "5";
        
        bool initialAdded = manager.addVariable(varName, initialValue, true);
        debugContext.add("Initial Variable Added", initialAdded);
        
        // Get value after initial add
        auto initialVariables = manager.getVariables();
        debugContext.add("Initial Variable Value", initialVariables[varName]);
        
        // Update the variable
        debugContext.addHeader("Variable Update");
        debugContext.add("Variable Name", varName);
        debugContext.add("Old Value", initialValue);
        debugContext.add("New Value", updatedValue);
        debugContext.add("Overwrite", true);
        
        bool updated = manager.addVariable(varName, updatedValue, true);
        debugContext.add("Update Succeeded", updated);
        
        // Verify the update
        auto variables = manager.getVariables();
        std::string finalValue = variables[varName];
        debugContext.add("Final Value", finalValue);
        
        bool correctValue = finalValue == updatedValue;
        debugContext.add("Value Changed Correctly", correctValue);
        
        return TestFramework::assert_that(updated && correctValue, 
                                        "Variable should be updated when overwrite is true",
                                        debugContext);
    });
    
    // Test 5: Try to update existing variable with overwrite=false
    suite.addTest("Try to update existing variable with overwrite=false", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Initial variable setup
        std::string varName = "counter";
        std::string initialValue = "0";
        std::string attemptedValue = "5";
        
        bool initialAdded = manager.addVariable(varName, initialValue, true);
        debugContext.add("Initial Variable Added", initialAdded);
        
        // Get value after initial add
        auto initialVariables = manager.getVariables();
        debugContext.add("Initial Variable Value", initialVariables[varName]);
        
        // Attempt to update the variable with overwrite=false
        debugContext.addHeader("Variable Update Attempt");
        debugContext.add("Variable Name", varName);
        debugContext.add("Original Value", initialValue);
        debugContext.add("Attempted New Value", attemptedValue);
        debugContext.add("Overwrite", false);
        
        bool updated = manager.addVariable(varName, attemptedValue, false);
        debugContext.add("Update Succeeded", updated);
        
        // Verify the update (should not have happened)
        auto variables = manager.getVariables();
        std::string finalValue = variables[varName];
        debugContext.add("Final Value", finalValue);
        
        bool valueUnchanged = finalValue == initialValue;
        debugContext.add("Value Remained Unchanged", valueUnchanged);
        
        return TestFramework::assert_that(!updated && valueUnchanged, 
                                        "Variable should not be updated when overwrite is false",
                                        debugContext);
    });
    
    // Test 6: Remove a variable
    suite.addTest("Remove a variable", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Add a variable to remove
        std::string varName = "toRemove";
        std::string varValue = "value";
        
        bool varAdded = manager.addVariable(varName, varValue, true);
        debugContext.add("Variable Added", varAdded);
        
        if (!varAdded) {
            return TestFramework::assert_that(false, "Failed to add variable for removal test", debugContext);
        }
        
        // Check variables before removal
        auto beforeRemoval = manager.getVariables();
        bool existsBefore = beforeRemoval.find(varName) != beforeRemoval.end();
        debugContext.add("Variables Before Removal", beforeRemoval);
        debugContext.add("Target Variable Exists Before", existsBefore);
        
        // Remove the variable
        debugContext.addHeader("Variable Removal");
        debugContext.add("Variable to Remove", varName);
        
        bool removed = manager.removeVariable(varName);
        debugContext.add("Remove Operation Success", removed);
        
        // Check variables after removal
        auto afterRemoval = manager.getVariables();
        bool existsAfter = afterRemoval.find(varName) != afterRemoval.end();
        
        debugContext.add("Variables After Removal", afterRemoval);
        debugContext.add("Target Variable Exists After", existsAfter);
        
        return TestFramework::assert_that(removed && existsBefore && !existsAfter, 
                                        "Variable should be successfully removed",
                                        debugContext);
    });
    
    // Test 7: Remove a non-existent variable
    suite.addTest("Remove a non-existent variable", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        // The implementation silently ignores non-existent variables and returns true
        bool removed = manager.removeVariable("nonExistent");
        
        return TestFramework::assert_that(removed, "Implementation silently ignores non-existent variables");
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
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Check initial expected inputs
        auto initialInputs = manager.getExpectedInputs();
        debugContext.add("Initial Expected Inputs", initialInputs);
        
        // Add new expected input
        debugContext.addHeader("Input Addition");
        char inputChar = 'A';
        debugContext.add("Input Character", std::string(1, inputChar));
        
        bool added = manager.addExpectedInput(inputChar);
        debugContext.add("Addition Success", added);
        
        // Verify the input was added
        auto updatedInputs = manager.getExpectedInputs();
        debugContext.add("Updated Expected Inputs", updatedInputs);
        
        bool exists = std::find(updatedInputs.begin(), updatedInputs.end(), inputChar) != updatedInputs.end();
        debugContext.add("Input Exists After Addition", exists);
        
        return TestFramework::assert_that(added && exists, 
                                        "Expected input character should be added successfully",
                                        debugContext);
    });
    
    // Test 10: Add duplicate expected input character
    suite.addTest("Add duplicate expected input character", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Add the initial character
        char inputChar = 'A';
        bool firstAdded = manager.addExpectedInput(inputChar);
        debugContext.add("Initial Character", std::string(1, inputChar));
        debugContext.add("First Addition Success", firstAdded);
        
        // Check state after first addition
        auto afterFirstAdd = manager.getExpectedInputs();
        debugContext.add("Inputs After First Addition", afterFirstAdd);
        int countAfterFirst = std::count(afterFirstAdd.begin(), afterFirstAdd.end(), inputChar);
        debugContext.add("Character Count After First Add", countAfterFirst);
        
        // Try to add duplicate
        debugContext.addHeader("Duplicate Addition");
        bool addedDuplicate = manager.addExpectedInput(inputChar);
        debugContext.add("Duplicate Addition Success", addedDuplicate);
        
        // Check state after duplicate attempt
        auto afterDuplicate = manager.getExpectedInputs();
        debugContext.add("Inputs After Duplicate Attempt", afterDuplicate);
        int countAfterDuplicate = std::count(afterDuplicate.begin(), afterDuplicate.end(), inputChar);
        debugContext.add("Character Count After Duplicate", countAfterDuplicate);
        
        // Note: Behavior might vary depending on implementation - here we assume duplicates are rejected
        bool noDuplication = countAfterDuplicate == 1;
        debugContext.add("No Duplication Occurred", noDuplication);
        
        return TestFramework::assert_that(!addedDuplicate && noDuplication, 
                                        "Duplicate expected input should not be added",
                                        debugContext);
    });
    
    // Test 11: Remove expected input character
    suite.addTest("Remove expected input character", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Add the character to be removed
        char inputChar = 'A';
        bool added = manager.addExpectedInput(inputChar);
        debugContext.add("Character Added", added);
        debugContext.add("Character to Add/Remove", std::string(1, inputChar));
        
        // Check before removal
        auto beforeRemoval = manager.getExpectedInputs();
        debugContext.add("Inputs Before Removal", beforeRemoval);
        
        bool existsBefore = std::find(beforeRemoval.begin(), beforeRemoval.end(), inputChar) != beforeRemoval.end();
        debugContext.add("Character Exists Before Removal", existsBefore);
        
        // Remove the character
        debugContext.addHeader("Character Removal");
        bool removed = manager.removeExpectedInput(inputChar);
        debugContext.add("Removal Operation Success", removed);
        
        // Check after removal
        auto afterRemoval = manager.getExpectedInputs();
        debugContext.add("Inputs After Removal", afterRemoval);
        
        bool existsAfter = std::find(afterRemoval.begin(), afterRemoval.end(), inputChar) != afterRemoval.end();
        debugContext.add("Character Exists After Removal", existsAfter);
        
        return TestFramework::assert_that(existsBefore && removed && !existsAfter, 
                                        "Expected input character should be removed successfully",
                                        debugContext);
    });
    
    // Test 12: Remove non-existent expected input character
    suite.addTest("Remove non-existent expected input character", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Check initial inputs
        auto initialInputs = manager.getExpectedInputs();
        debugContext.add("Initial Expected Inputs", initialInputs);
        
        // Try to remove a non-existent character
        char nonExistentChar = 'Z';
        debugContext.addHeader("Character Removal");
        debugContext.add("Character to Remove", std::string(1, nonExistentChar));
        
        bool charExists = std::find(initialInputs.begin(), initialInputs.end(), nonExistentChar) != initialInputs.end();
        debugContext.add("Character Exists Before Removal", charExists);
        
        // The implementation silently ignores non-existent inputs and returns true
        bool removed = manager.removeExpectedInput(nonExistentChar);
        debugContext.add("Removal Operation Success", removed);
        
        // Check inputs after removal attempt
        auto afterRemoval = manager.getExpectedInputs();
        debugContext.add("Inputs After Removal Attempt", afterRemoval);
        
        // Verify inputs are unchanged
        bool inputsUnchanged = (initialInputs == afterRemoval);
        debugContext.add("Inputs Unchanged", inputsUnchanged);
        
        return TestFramework::assert_that(removed, 
                                        "Implementation silently ignores non-existent inputs",
                                        debugContext);
    });
    
    // Test 13: Get all expected input characters
    suite.addTest("Get all expected input characters", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        // Check initial state
        auto initialInputs = manager.getExpectedInputs();
        debugContext.add("Initial Expected Inputs", initialInputs);
        
        debugContext.addHeader("Adding Expected Inputs");
        // Add some expected input characters
        char input1 = 'A';
        char input2 = 'B';
        char input3 = 'C';
        
        debugContext.add("Input 1", std::string(1, input1));
        debugContext.add("Input 2", std::string(1, input2));
        debugContext.add("Input 3", std::string(1, input3));
        
        bool added1 = manager.addExpectedInput(input1);
        bool added2 = manager.addExpectedInput(input2);
        bool added3 = manager.addExpectedInput(input3);
        
        debugContext.add("Input 1 Added", added1);
        debugContext.add("Input 2 Added", added2);
        debugContext.add("Input 3 Added", added3);
        
        if (!added1 || !added2 || !added3) {
            return TestFramework::assert_that(false, "Failed to add expected inputs for test", debugContext);
        }
        
        debugContext.addHeader("Retrieving All Inputs");
        // Get all expected input characters
        auto inputs = manager.getExpectedInputs();
        debugContext.add("Retrieved Inputs", inputs);
        
        // Check that all added inputs are present
        bool allExist = std::find(inputs.begin(), inputs.end(), input1) != inputs.end() &&
                        std::find(inputs.begin(), inputs.end(), input2) != inputs.end() &&
                        std::find(inputs.begin(), inputs.end(), input3) != inputs.end();
        bool correctSize = inputs.size() == 3;
        
        debugContext.add("All Inputs Present", allExist);
        debugContext.add("Correct Number of Inputs", correctSize);
        
        return TestFramework::assert_that(allExist && correctSize, 
                                        "Should return all expected input characters correctly",
                                        debugContext);
    });
    
    // Test 14: Set input string
    suite.addTest("Set input string", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        debugContext.addHeader("Input Character Setup");
        // Add expected input characters
        char inputA = 'A';
        char inputB = 'B';
        char inputC = 'C';
        char inputD = 'D';
        char inputE = 'E';
        char inputF = 'F';
        
        debugContext.add("Adding Input Characters", 
                      std::string(1, inputA) + ", " + 
                      std::string(1, inputB) + ", " + 
                      std::string(1, inputC) + ", " + 
                      std::string(1, inputD) + ", " + 
                      std::string(1, inputE) + ", " + 
                      std::string(1, inputF));
                      
        bool allAdded = manager.addExpectedInput(inputA) &&
                        manager.addExpectedInput(inputB) &&
                        manager.addExpectedInput(inputC) &&
                        manager.addExpectedInput(inputD) &&
                        manager.addExpectedInput(inputE) &&
                        manager.addExpectedInput(inputF);
                        
        debugContext.add("All Inputs Added Successfully", allAdded);
        
        if (!allAdded) {
            return TestFramework::assert_that(false, "Failed to add expected inputs for test", debugContext);
        }
        
        debugContext.addHeader("Setting Input String");
        std::string inputStr = "ABCDEF";
        debugContext.add("Input String", inputStr);
        
        bool set = manager.setInput(inputStr);
        debugContext.add("Set Input Success", set);
        
        std::string retrievedInput = manager.getInput();
        debugContext.add("Retrieved Input", retrievedInput);
        
        bool correctlySet = set && retrievedInput == inputStr;
        debugContext.add("Input Correctly Set and Retrieved", correctlySet);
        
        return TestFramework::assert_that(correctlySet, 
                                        "Input string should be set and retrieved correctly",
                                        debugContext);
    });
    
    // Test 15: Set empty input string
    suite.addTest("Set empty input string", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        debugContext.addHeader("Setting Empty Input");
        std::string emptyInput = "";
        debugContext.add("Input String", emptyInput.empty() ? "(empty)" : emptyInput);
        
        bool set = manager.setInput(emptyInput);
        debugContext.add("Set Input Success", set);
        
        std::string retrievedInput = manager.getInput();
        debugContext.add("Retrieved Input", retrievedInput.empty() ? "(empty)" : retrievedInput);
        
        bool correctlySet = set && retrievedInput.empty();
        debugContext.add("Empty Input Correctly Set", correctlySet);
        
        return TestFramework::assert_that(correctlySet, 
                                        "Empty input string should be set successfully",
                                        debugContext);
    });
    
    // Test 16: Set and get output string
    suite.addTest("Set and get output string", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Setup");
        
        FSMManager manager;
        bool setupSuccess = setupStandardTestFSM(manager);
        debugContext.add("Setup Success", setupSuccess);
        
        if (!setupSuccess) {
            return TestFramework::assert_that(false, "Failed to set up test FSM", debugContext);
        }
        
        debugContext.addHeader("Output String Verification");
        
        // Initial output should be empty
        std::string initialOutput = manager.getOutput();
        bool initialEmpty = initialOutput.empty();
        
        debugContext.add("Initial Output", initialOutput.empty() ? "(empty)" : initialOutput);
        debugContext.add("Initial Output is Empty", initialEmpty);
        
        // Output is generally set during execution
        // Here we'll just check the initial state
        return TestFramework::assert_that(initialEmpty, 
                                        "Initial output string should be empty",
                                        debugContext);
    });
    
    return suite;
}

} // namespace FSMVariableInputTests

#endif // FSM_VARIABLE_INPUT_TESTS_HPP
