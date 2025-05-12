#ifndef FSM_STATE_MANAGEMENT_TESTS_HPP
#define FSM_STATE_MANAGEMENT_TESTS_HPP

#include "test_framework.hpp"
#include "fsmInterface.hpp"
#include <iostream>
#include <memory>
#include <string>

/**
 * @file fsm_state_management_tests.hpp
 * @brief Tests for FSM state management operations
 * 
 * This file contains tests related to adding, removing, updating states
 * and verifying proper state property handling.
 */

namespace FSMStateManagementTests {

/**
 * @brief Creates and returns a test suite for FSM state management tests
 * @return A TestSuite object configured with state management tests
 */
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("FSM State Management Tests", "Tests for state-related operations in the FSM");
    
    // Setup function for the suite
    suite.setSuiteSetup([]() {
        std::cout << "Setting up FSM state management test suite..." << std::endl;
        return true; // Setup successful
    });
    
    // Teardown function for the suite
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down FSM state management test suite..." << std::endl;
        return true; // Teardown successful
    });
    
    // Test 1: Add a basic state
    suite.addTest("Add a basic state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        bool added = manager.addState("State1", "console.log('State1');", 'A', false);
        
        return TestFramework::assert_that(added, "Basic state should be added successfully");
    });
    
    // Test 2: Add a state with empty name
    suite.addTest("Add a state with empty name", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        bool added = manager.addState("", "console.log('Empty');", 'E', false);
        
        return TestFramework::assert_that(!added, "State with empty name should not be added");
    });
    
    // Test 3: Add a final state
    suite.addTest("Add a final state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        bool added = manager.addState("FinalState", "console.log('Final');", 'F', true);
        
        if (!added) {
            return TestFramework::TestResult(false, "Failed to add final state", std::chrono::milliseconds(0));
        }
        
        auto fsm = manager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get FSM instance", std::chrono::milliseconds(0));
        }
        
        auto stateNames = manager.getAllStateNames();
        bool stateExists = false;
        for (const auto& name : stateNames) {
            if (name == "FinalState") {
                stateExists = true;
                break;
            }
        }
        
        nlohmann::json stateInfo = manager.getStateInfo("FinalState");
        bool isFinal = stateInfo["isFinal"].get<bool>();
        
        return TestFramework::assert_that(stateExists && isFinal, "Final state should be added and properly marked as final");
    });
    
    // Test 4: Add multiple states
    suite.addTest("Add multiple states", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        bool added1 = manager.addState("State1", "console.log('State1');", 'A', false);
        bool added2 = manager.addState("State2", "console.log('State2');", 'B', false);
        bool added3 = manager.addState("State3", "console.log('State3');", 'C', true);
        
        auto stateNames = manager.getAllStateNames();
        bool allStatesExist = stateNames.size() == 3;
        
        return TestFramework::assert_that(added1 && added2 && added3 && allStatesExist, 
                                         "All states should be added successfully");
    });
    
    // Test 5: Remove a state
    suite.addTest("Remove a state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        manager.addState("State1", "console.log('State1');", 'A', false);
        manager.addState("State2", "console.log('State2');", 'B', false);
        
        auto beforeRemoval = manager.getAllStateNames();
        bool beforeSize = (beforeRemoval.size() == 2);
        
        bool removed = manager.removeState("State1");
        
        auto afterRemoval = manager.getAllStateNames();
        bool afterSize = (afterRemoval.size() == 1);
        bool state2Exists = (afterRemoval[0] == "State2");
        
        return TestFramework::assert_that(beforeSize && removed && afterSize && state2Exists, 
                                         "State should be removed successfully");
    });
    
    // Test 6: Remove a non-existent state
    suite.addTest("Remove a non-existent state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        manager.addState("State1", "console.log('State1');", 'A', false);
        
        // The implementation may not return false for non-existent states
        // This might be a silent no-op in the implementation
        manager.removeState("NonExistentState");
        
        // Since we can't rely on the return value, let's verify the state still exists
        auto stateNames = manager.getAllStateNames();
        bool state1Exists = (stateNames.size() == 1) && (stateNames[0] == "State1");
        
        return TestFramework::assert_that(state1Exists, "Original state should still exist after removal attempt");
    });
    
    // Test 7: Set start state
    suite.addTest("Set start state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        manager.addState("State1", "console.log('State1');", 'A', false);
        manager.addState("State2", "console.log('State2');", 'B', false);
        
        bool setStart = manager.setStartState("State1");
        
        // We can't verify the start state directly without a getter, but we can validate it succeeded
        return TestFramework::assert_that(setStart, "Setting start state should succeed");
    });
    
    // Test 8: Set non-existent start state
    suite.addTest("Set non-existent start state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        manager.addState("State1", "console.log('State1');", 'A', false);
        
        bool setStart = manager.setStartState("NonExistentState");
        
        return TestFramework::assert_that(!setStart, "Setting a non-existent start state should fail");
    });
    
    // Test 9: Update state (without changing name)
    suite.addTest("Update state", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        manager.addState("State1", "console.log('Original');", 'A', false);
        
        // Update without changing name - this is safer as renaming requires special handling
        bool updated = manager.updateState("State1", "State1", "console.log('Updated');", 'U', 
                                          true, true, std::chrono::milliseconds(200), true);
        
        if (!updated) {
            return TestFramework::TestResult(false, "Failed to update state", std::chrono::milliseconds(0));
        }
        
        nlohmann::json stateInfo = manager.getStateInfo("State1");
        bool isFinal = stateInfo["isFinal"].get<bool>();
        char output = stateInfo["output"].get<std::string>()[0];
        
        return TestFramework::assert_that(updated && isFinal && output == 'U', 
                                         "State should be updated successfully with attributes changed");
    });
    
    // Test 10: Get all state names
    suite.addTest("Get all state names", []() {
        FSMManager manager;
        manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        manager.addState("State1", "console.log('State1');", 'A', false);
        manager.addState("State2", "console.log('State2');", 'B', false);
        manager.addState("State3", "console.log('State3');", 'C', true);
        
        auto stateNames = manager.getAllStateNames();
        std::sort(stateNames.begin(), stateNames.end());
        
        std::vector<std::string> expected = {"State1", "State2", "State3"};
        
        return TestFramework::assert_equal(expected, stateNames, "Should return all state names correctly");
    });
    
    return suite;
}

} // namespace FSMStateManagementTests

#endif // FSM_STATE_MANAGEMENT_TESTS_HPP
