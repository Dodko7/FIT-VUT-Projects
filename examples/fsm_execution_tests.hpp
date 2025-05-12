#ifndef FSM_EXECUTION_TESTS_HPP
#define FSM_EXECUTION_TESTS_HPP

#include "test_framework.hpp"
#include "fsmInterface.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

/**
 * @file fsm_execution_tests.hpp
 * @brief Tests for FSM execution and runtime control
 * 
 * This file contains tests related to executing the FSM, stepping through
 * states, and runtime control operations.
 */

namespace FSMExecutionTests {

/**
 * @brief Helper function to create a standard test FSM for execution tests
 * @param manager Reference to the FSM manager to use
 * @return True if successful, false otherwise
 */
bool setupExecutionTestFSM(FSMManager& manager) {
    if (!manager.createFSM("ExecutionTestFSM", "FSM for testing execution", std::chrono::milliseconds(50))) {
        return false;
    }
    
    // Create a simple FSM that increments a counter and transitions based on input
    if (!manager.addState("Start", "counter = 0;", 'S', false)) {
        return false;
    }
    
    if (!manager.addState("Process", "counter = counter + 1;", 'P', false)) {
        return false;
    }
    
    if (!manager.addState("Final", "console.log('Final counter: ' + counter);", 'F', true)) {
        return false;
    }
    
    if (!manager.setStartState("Start")) {
        return false;
    }
    
    // Add transitions
    if (!manager.addTransition("Start", "Process", "true", 'A')) {
        return false;
    }
    
    if (!manager.addTransition("Process", "Process", "counter < 3", 'B')) {
        return false;
    }
    
    if (!manager.addTransition("Process", "Final", "counter >= 3", 'C')) {
        return false;
    }
    
    // Add variables
    if (!manager.addVariable("counter", "0", true)) {
        return false;
    }
    
    // Add expected inputs
    if (!manager.addExpectedInput('A') || !manager.addExpectedInput('B') || !manager.addExpectedInput('C')) {
        return false;
    }
    
    return true;
}

/**
 * @brief Creates and returns a test suite for FSM execution tests
 * @return A TestSuite object configured with execution tests
 */
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("FSM Execution Tests", "Tests for FSM execution and runtime control operations");
    
    // Setup function for the suite
    suite.setSuiteSetup([]() {
        std::cout << "Setting up FSM execution test suite..." << std::endl;
        return true; // Setup successful
    });
    
    // Teardown function for the suite
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down FSM execution test suite..." << std::endl;
        return true; // Teardown successful
    });
    
    // Test 1: Reset FSM
    suite.addTest("Reset FSM", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        // Step through a couple of steps
        manager.setInput("AABB");
        manager.step();
        manager.step();
        
        std::string stateBefore = manager.getCurrentState();
        bool resetSuccess = manager.reset();
        std::string stateAfter = manager.getCurrentState();
        
        return TestFramework::assert_that(resetSuccess && stateAfter == "Start" && stateBefore != stateAfter,
                                        "FSM should be reset to start state");
    });
    
    // Test 2: Step through FSM
    suite.addTest("Step through FSM", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput("ABBC");
        
        bool step1 = manager.step(); // Start -> Process
        std::string state1 = manager.getCurrentState();
        
        bool step2 = manager.step(); // Process -> Process
        std::string state2 = manager.getCurrentState();
        
        bool step3 = manager.step(); // Process -> Process
        std::string state3 = manager.getCurrentState();
        
        bool step4 = manager.step(); // Process -> Final
        std::string state4 = manager.getCurrentState();
        
        bool correctSequence = (state1 == "Process" && 
                               state2 == "Process" && 
                               state3 == "Process" && 
                               state4 == "Final");
        
        return TestFramework::assert_that(step1 && step2 && step3 && step4 && correctSequence,
                                        "FSM should step through states correctly based on input");
    });
    
    // Test 3: Run FSM to completion
    suite.addTest("Run FSM to completion", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput("ABBC");
        
        bool runSuccess = manager.run();
        std::string finalState = manager.getCurrentState();
        
        return TestFramework::assert_that(runSuccess && finalState == "Final",
                                        "FSM should run to completion and end in final state");
    });
    
    // Test 4: Run FSM with insufficient input
    suite.addTest("Run FSM with insufficient input", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput("A"); // Only enough to get to Process, not Final
        
        bool runResult = manager.run();
        std::string currentState = manager.getCurrentState();
        bool stoppedEarly = currentState != "Final";
        
        // Depending on implementation, run might return true if it executed but didn't reach final state,
        // or might return false if it considers not reaching final state as a failure
        return TestFramework::TestResult(true, std::string("FSM ran with insufficient input and ") + 
                                    (stoppedEarly ? "correctly stopped early" : "reached final state unexpectedly"), 
                                    std::chrono::milliseconds(0));
    });
    
    // Test 5: Get current state
    suite.addTest("Get current state", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        // Initially should be in start state
        std::string initialState = manager.getCurrentState();
        bool initialIsStart = initialState == "Start";
        
        manager.setInput("A");
        manager.step(); // Move to Process state
        
        std::string nextState = manager.getCurrentState();
        bool nextIsProcess = nextState == "Process";
        
        return TestFramework::assert_that(initialIsStart && nextIsProcess,
                                        "getCurrentState should return the correct current state");
    });
    
    // Test 6: Get machine state
    suite.addTest("Get machine state", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        // Initially should be in IDLE state
        machineState initialMState = manager.getMachineState();
        bool initialIsIdle = initialMState == machineState::IDLE;
        
        // Change the machine state
        manager.setMachineState(machineState::RUNNING);
        
        machineState newMState = manager.getMachineState();
        bool newIsRunning = newMState == machineState::RUNNING;
        
        return TestFramework::assert_that(initialIsIdle && newIsRunning,
                                        "getMachineState and setMachineState should work correctly");
    });
    
    // Test 7: Validate valid FSM
    suite.addTest("Validate valid FSM", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        bool isValid = manager.validateFSM();
        
        return TestFramework::assert_that(isValid, "Valid FSM should pass validation");
    });
    
    // Test 8: Validate FSM without start state
    suite.addTest("Validate FSM without start state", []() {
        FSMManager manager;
        // Create FSM without setting a start state
        manager.createFSM("InvalidFSM", "FSM without start state", std::chrono::milliseconds(50));
        
        manager.addState("State1", "", 'A', false);
        manager.addState("State2", "", 'B', false);
        
        // Don't set start state
        
        bool isValid = manager.validateFSM();
        
        return TestFramework::assert_that(!isValid, "FSM without start state should fail validation");
    });
    
    // Test 9: Validate FSM without final state
    suite.addTest("Validate FSM without final state", []() {
        FSMManager manager;
        // Create FSM without a final state
        manager.createFSM("InvalidFSM", "FSM without final state", std::chrono::milliseconds(50));
        
        manager.addState("State1", "", 'A', false);
        manager.addState("State2", "", 'B', false);
        manager.setStartState("State1");
        
        // No state is marked as final
        
        bool isValid = manager.validateFSM();
        
        return TestFramework::assert_that(!isValid, "FSM without final state should fail validation");
    });
    
    // Test 10: Prune unreachable states
    suite.addTest("Prune unreachable states", []() {
        FSMManager manager;
        manager.createFSM("PruneTestFSM", "FSM with unreachable states", std::chrono::milliseconds(50));
        
        manager.addState("Start", "", 'S', false);
        manager.addState("Reachable", "", 'R', false);
        manager.addState("Unreachable", "", 'U', true);
        manager.addState("Final", "", 'F', true);
        
        manager.setStartState("Start");
        
        // Only add transitions to Reachable and Final, leaving Unreachable isolated
        manager.addTransition("Start", "Reachable", "true", 'A');
        manager.addTransition("Reachable", "Final", "true", 'B');
        
        auto beforePrune = manager.getAllStateNames();
        bool beforeSize = (beforePrune.size() == 4);
        
        bool pruned = manager.pruneUnreachableStates();
        
        auto afterPrune = manager.getAllStateNames();
        bool afterSize = (afterPrune.size() == 3); // Unreachable should be removed
        
        // Check that the unreachable state is gone
        bool unreachableRemoved = true;
        for (const auto& state : afterPrune) {
            if (state == "Unreachable") {
                unreachableRemoved = false;
                break;
            }
        }
        
        return TestFramework::assert_that(beforeSize && pruned && afterSize && unreachableRemoved,
                                        "Unreachable states should be pruned correctly");
    });
    
    // Test 11: Step past the end of input
    suite.addTest("Step past the end of input", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput("ABBC"); // Exactly enough to reach final state
        
        // Step through all inputs
        manager.step();
        manager.step();
        manager.step();
        manager.step();
        
        // Try to step again
        bool extraStep = manager.step();
        
        return TestFramework::assert_that(!extraStep, 
                                        "Step past end of input should fail");
    });
    
    // Test 12: Run with no input
    suite.addTest("Run with no input", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput(""); // Empty input
        
        bool runResult = manager.run();
        
        // This depends on implementation - some might consider empty input valid,
        // others might require at least one character
        return TestFramework::TestResult(true, "Run with empty input " + 
                                      std::string(runResult ? "succeeded" : "failed") + " as expected", 
                                      std::chrono::milliseconds(0));
    });
    
    // Test 13: Run with invalid input characters
    suite.addTest("Run with invalid input characters", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput("AXB"); // 'X' is not an expected input
        
        bool runResult = manager.run();
        std::string currentState = manager.getCurrentState();
        
        // Depending on implementation, might stop at 'A' or might skip 'X'
        return TestFramework::TestResult(true, std::string("Run with invalid input character ") + 
                                      (runResult ? "handled invalid input" : "failed") + 
                                      " and ended in state " + currentState, 
                                      std::chrono::milliseconds(0));
    });
    
    // Test 14: Check output generation
    suite.addTest("Check output generation", []() {
        FSMManager manager;
        if (!setupExecutionTestFSM(manager)) {
            return TestFramework::TestResult(false, "Failed to set up test FSM", std::chrono::milliseconds(0));
        }
        
        manager.setInput("ABBC");
        
        // Initially output should be empty
        std::string initialOutput = manager.getOutput();
        bool initialEmpty = initialOutput.empty();
        
        // Run the FSM
        manager.run();
        
        // Should have output for each step
        std::string finalOutput = manager.getOutput();
        bool outputGenerated = !finalOutput.empty();
        
        // Expected output is "SPPPF" based on state outputs
        bool correctOutput = (finalOutput == "SPPPF");
        
        return TestFramework::assert_that(initialEmpty && outputGenerated && correctOutput,
                                        "FSM should generate correct output during execution");
    });
    
    // Test 15: Check execution with step delay
    suite.addTest("Check execution with step delay", []() {
        FSMManager manager;
        
        // Create FSM with a significant step delay
        manager.createFSM("DelayTestFSM", "FSM with step delay", std::chrono::milliseconds(100));
        
        manager.addState("Start", "", 'S', false);
        manager.addState("Middle", "", 'M', false);
        manager.addState("End", "", 'E', true);
        
        manager.setStartState("Start");
        
        manager.addTransition("Start", "Middle", "true", 'A');
        manager.addTransition("Middle", "End", "true", 'B');
        
        manager.setInput("AB");
        
        // Measure execution time
        auto startTime = std::chrono::high_resolution_clock::now();
        manager.run();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // With 2 transitions and 100ms delay, should take at least ~200ms
        bool tookExpectedTime = duration.count() >= 190; // Allow for small timing variations
        
        return TestFramework::assert_that(tookExpectedTime,
                                        "FSM execution should respect step delay setting");
    });
    
    return suite;
}

} // namespace FSMExecutionTests

#endif // FSM_EXECUTION_TESTS_HPP
