#include "test_framework.hpp"
#include "fsm_creation_tests.hpp"
#include "fsm_state_management_tests.hpp"
#include "fsm_file_operations_tests.hpp"
#include "fsm_transition_management_tests.hpp"
#include "fsm_variable_input_tests.hpp"
#include "fsm_execution_tests.hpp"

#include <iostream>
#include <string>
#include <chrono>

/**
 * @file fsm_test_runner.cpp
 * @brief Main test runner for FSM implementation tests
 * 
 * This file runs all test suites for the FSM implementation and 
 * generates a comprehensive report of the test results.
 */

int main(int argc, char* argv[]) {
    std::cout << "===============================================" << std::endl;
    std::cout << "        FSM Implementation Test Runner         " << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create test runner
    TestFramework::TestRunner runner;
    
    // Add all test suites
    runner.addSuite(FSMCreationTests::createTestSuite());
    runner.addSuite(FSMStateManagementTests::createTestSuite());
    runner.addSuite(FSMFileOperationsTests::createTestSuite());
    runner.addSuite(FSMTransitionManagementTests::createTestSuite());
    runner.addSuite(FSMVariableInputTests::createTestSuite());
    runner.addSuite(FSMExecutionTests::createTestSuite());
    
    // Run all tests
    bool allPassed = runner.runAllSuites();
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Generate report
    std::string reportFilePath = "fsm_test_report.txt";
    std::string report = runner.generateReport(reportFilePath);
    
    // Print final summary to console
    std::cout << "\n===============================================" << std::endl;
    std::cout << "                TEST SUMMARY                   " << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "Total Test Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "Overall Result: " << (allPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Detailed report written to: " << reportFilePath << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    // Print machine-readable result line for CI integration
    std::cout << "TEST_RESULT:" << (allPassed ? "PASS" : "FAIL") << std::endl;
    
    return allPassed ? 0 : 1;
}
