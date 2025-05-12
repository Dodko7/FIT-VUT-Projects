#ifndef FSM_TRANSITION_MANAGEMENT_TESTS_HPP
#define FSM_TRANSITION_MANAGEMENT_TESTS_HPP

#include "test_framework.hpp"
#include "fsmInterface.hpp"
#include <iostream>
#include <memory>
#include <string>

/**
 * @file fsm_transition_management_tests.hpp
 * @brief Tests for FSM transition management
 * 
 * This file contains tests related to adding, removing, and updating
 * transitions between states and verifying proper transition behavior.
 */

namespace FSMTransitionManagementTests {

/**
 * @brief Helper function to create a standard test FSM with states
 * @param manager Reference to the FSM manager to use
 * @return True if successful, false otherwise
 */
bool setupStandardTestFSM(FSMManager& manager) {
    if (!manager.createFSM("TransitionTestFSM", "FSM for testing transitions", std::chrono::milliseconds(100))) {
        return false;
    }
    
    if (!manager.addState("State1", "console.log('State1');", 'A', false)) {
        return false;
    }
    
    if (!manager.addState("State2", "console.log('State2');", 'B', false)) {
        return false;
    }
    
    if (!manager.addState("State3", "console.log('State3');", 'C', true)) {
        return false;
    }
    
    if (!manager.setStartState("State1")) {
        return false;
    }
    
    return true;
}

/**
 * @brief Creates and returns a test suite for FSM transition management tests
 * @return A TestSuite object configured with transition management tests
 */
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("FSM Transition Management Tests", "Tests for transition-related operations in the FSM");
    
    // Setup function for the suite
    suite.setSuiteSetup([]() {
        std::cout << "Setting up FSM transition management test suite..." << std::endl;
        return true; // Setup successful
    });
    
    // Teardown function for the suite
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down FSM transition management test suite..." << std::endl;
        return true; // Teardown successful
    });
    
    // Test 1: Add a basic transition
    suite.addTest("Add a basic transition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addTransition("State1", "State2", "true", '1');
        
        return TestFramework::assert_that(added, "Basic transition should be added successfully");
    });
    
    // Test 2: Add a transition with complex condition
    suite.addTest("Add a transition with complex condition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addTransition("State1", "State2", "x > 5 && y < 10 || z == true", '1');
        
        return TestFramework::assert_that(added, "Transition with complex condition should be added successfully");
    });
    
    // Test 3: Add a transition with empty condition
    suite.addTest("Add a transition with empty condition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addTransition("State1", "State2", "", '1');
        
        // Implementation may vary on whether empty conditions are acceptable
        // This assertion checks the expected behavior
        return TestFramework::assert_that(!added, "Transition with empty condition should fail to be added");
    });
    
    // Test 4: Add transition from non-existent state
    suite.addTest("Add transition from non-existent state", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addTransition("NonExistentState", "State2", "true", '1');
        
        return TestFramework::assert_that(!added, "Adding transition from non-existent state should fail");
    });
    
    // Test 5: Add transition to non-existent state
    suite.addTest("Add transition to non-existent state", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added = manager.addTransition("State1", "NonExistentState", "true", '1');
        
        return TestFramework::assert_that(!added, "Adding transition to non-existent state should fail");
    });
    
    // Test 6: Add duplicate transition (same from, to, input)
    suite.addTest("Add duplicate transition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool added1 = manager.addTransition("State1", "State2", "condition1", '1');
        bool added2 = manager.addTransition("State1", "State2", "condition2", '1');
        
        return TestFramework::assert_that(added1 && !added2, 
                                        "First transition should be added, duplicate should fail");
    });
    
    // Test 7: Remove a transition
    suite.addTest("Remove a transition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addTransition("State1", "State2", "true", '1');
        
        nlohmann::json beforeRemoval = manager.getStateTransitions("State1");
        bool beforeNonEmpty = !beforeRemoval.empty();
        
        bool removed = manager.removeTransition("State1", "State2", '1');
        
        nlohmann::json afterRemoval = manager.getStateTransitions("State1");
        bool afterEmpty = afterRemoval.empty() || 
                          (afterRemoval.is_array() && afterRemoval.size() == 0);
        
        return TestFramework::assert_that(beforeNonEmpty && removed && afterEmpty, 
                                        "Transition should be removed successfully");
    });
    
    // Test 8: Remove a non-existent transition
    suite.addTest("Remove a non-existent transition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool removed = manager.removeTransition("State1", "State2", '1');
        
        return TestFramework::assert_that(!removed, "Removing a non-existent transition should fail");
    });
    
    // Test 9: Get state transitions
    suite.addTest("Get state transitions", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addTransition("State1", "State2", "condition1", '1');
        manager.addTransition("State1", "State3", "condition2", '2');
        
        nlohmann::json transitions = manager.getStateTransitions("State1");
        
        bool hasTransitions = transitions.is_array() && transitions.size() == 2;
        
        return TestFramework::assert_that(hasTransitions, 
                                        "Should return correct number of transitions for a state");
    });
    
    // Test 10: Get transitions for a state with no transitions
    suite.addTest("Get transitions for state with no transitions", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        nlohmann::json transitions = manager.getStateTransitions("State1");
        
        bool isEmpty = transitions.empty() || 
                       (transitions.is_array() && transitions.size() == 0);
        
        return TestFramework::assert_that(isEmpty, 
                                        "Should return empty list for state with no transitions");
    });
    
    // Test 11: Get transitions for non-existent state
    suite.addTest("Get transitions for non-existent state", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        nlohmann::json transitions = manager.getStateTransitions("NonExistentState");
        
        bool isEmpty = transitions.empty() || 
                       (transitions.is_array() && transitions.size() == 0);
        
        return TestFramework::assert_that(isEmpty, 
                                        "Should return empty list for non-existent state");
    });
    
    // Test 12: Update transition
    suite.addTest("Update transition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.addTransition("State1", "State2", "oldCondition", '1');
        
        bool updated = manager.updateTransition("State1", "State2", '1', '2', "newCondition");
        
        // Need to verify the actual condition, but getStateTransitions doesn't directly give us that
        // So we check that update operation succeeded
        return TestFramework::assert_that(updated, 
                                        "Transition should be updated successfully");
    });
    
    // Test 13: Update non-existent transition
    suite.addTest("Update non-existent transition", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool updated = manager.updateTransition("State1", "State2", '1', '2', "newCondition");
        
        return TestFramework::assert_that(!updated, 
                                        "Updating non-existent transition should fail");
    });
    
    // Test 14: Create a cycle with transitions
    suite.addTest("Create a cycle with transitions", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool t1 = manager.addTransition("State1", "State2", "condition1", '1');
        bool t2 = manager.addTransition("State2", "State3", "condition2", '2');
        bool t3 = manager.addTransition("State3", "State1", "condition3", '3');
        
        return TestFramework::assert_that(t1 && t2 && t3, 
                                        "Should be able to create a cycle with transitions");
    });
    
    // Test 15: Add transitions with same input but different source states
    suite.addTest("Add transitions with same input but different source states", []() {
        FSMManager manager;
        if (!setupStandardTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool t1 = manager.addTransition("State1", "State2", "condition1", '1');
        bool t2 = manager.addTransition("State2", "State3", "condition2", '1');
        
        return TestFramework::assert_that(t1 && t2, 
                                        "Should be able to add transitions with same input but different sources");
    });
    
    return suite;
}

} // namespace FSMTransitionManagementTests

#endif // FSM_TRANSITION_MANAGEMENT_TESTS_HPP
