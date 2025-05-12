#ifndef FSM_CREATION_TESTS_HPP
#define FSM_CREATION_TESTS_HPP

#include "test_framework.hpp"
#include "fsmInterface.hpp"
#include <iostream>
#include <memory>
#include <string>

/**
 * @file fsm_creation_tests.hpp
 * @brief Tests for FSM creation, initialization, and basic properties
 * 
 * This file contains tests related to creating FSM instances, setting
 * their properties, and verifying their basic functionality.
 */

namespace FSMCreationTests {

/**
 * @brief Creates and returns a test suite for FSM creation tests
 * @return A TestSuite object configured with FSM creation tests
 */
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("FSM Creation Tests", "Tests for FSM creation, initialization, and basic properties");
    
    // Setup function for the suite
    suite.setSuiteSetup([]() {
        std::cout << "Setting up FSM creation test suite..." << std::endl;
        return true; // Setup successful
    });
    
    // Teardown function for the suite
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down FSM creation test suite..." << std::endl;
        return true; // Teardown successful
    });
    
    // Test 1: Create FSM with valid parameters
    suite.addTest("Create FSM with valid parameters", []() {
        FSMManager manager;
        bool created = manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
        
        return TestFramework::assert_that(created, "FSM should be created successfully");
    });
    
    // Test 2: Create FSM with empty name
    suite.addTest("Create FSM with empty name", []() {
        FSMManager manager;
        bool created = manager.createFSM("", "A test FSM", std::chrono::milliseconds(100));
        
        return TestFramework::assert_that(!created, "FSM creation should fail with empty name");
    });
    
    // Test 3: Create FSM with very long name
    suite.addTest("Create FSM with very long name", []() {
        FSMManager manager;
        std::string longName(1000, 'a'); // Create a string of 1000 'a's
        bool created = manager.createFSM(longName, "A test FSM", std::chrono::milliseconds(100));
        
        // Note: This depends on internal implementation, might need adjustment
        return TestFramework::assert_that(created, "FSM should handle a very long name");
    });
    
    // Test 4: Create FSM with negative step delay
    suite.addTest("Create FSM with negative step delay", []() {
        FSMManager manager;
        bool created = manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(-100));
        
        // Implementation-dependent: some systems handle negative values differently
        // Here we assume it fails or resets to a positive value
        // Test requires knowledge of actual expected behavior
        return TestFramework::assert_that(created, "FSM should handle negative delay appropriately");
    });
    
    // Test 5: Create FSM with zero step delay
    suite.addTest("Create FSM with zero step delay", []() {
        FSMManager manager;
        bool created = manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(0));
        
        return TestFramework::assert_that(created, "FSM should handle zero delay");
    });
    
    // Test 6: Create FSM with very large step delay
    suite.addTest("Create FSM with very large step delay", []() {
        FSMManager manager;
        bool created = manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(1000000));
        
        return TestFramework::assert_that(created, "FSM should handle very large delay");
    });
    
    // Test 7: Create FSM and verify its name
    suite.addTest("Create FSM and verify its name", []() {
        FSMManager manager;
        std::string fsmName = "NameCheckFSM";
        manager.createFSM(fsmName, "A test FSM", std::chrono::milliseconds(100));
        
        auto fsm = manager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get FSM instance", std::chrono::milliseconds(0));
        }
        
        std::string actualName = fsm->getName();
        return TestFramework::assert_equal(fsmName, actualName, "FSM name should match the one specified at creation");
    });
    
    // Test 8: Create FSM and verify its description
    suite.addTest("Create FSM and verify its description", []() {
        FSMManager manager;
        std::string fsmDesc = "Description Check FSM";
        manager.createFSM("DescCheckFSM", fsmDesc, std::chrono::milliseconds(100));
        
        auto fsm = manager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get FSM instance", std::chrono::milliseconds(0));
        }
        
        std::string actualDesc = fsm->getDescription();
        return TestFramework::assert_equal(fsmDesc, actualDesc, "FSM description should match the one specified at creation");
    });
    
    // Test 9: Create FSM and verify its step delay
    suite.addTest("Create FSM and verify its step delay", []() {
        FSMManager manager;
        auto stepDelay = std::chrono::milliseconds(250);
        manager.createFSM("DelayCheckFSM", "A test FSM", stepDelay);
        
        auto fsm = manager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get FSM instance", std::chrono::milliseconds(0));
        }
        
        auto actualDelay = fsm->getStepDelay();
        return TestFramework::assert_equal(stepDelay, actualDelay, "FSM step delay should match the one specified at creation");
    });
    
    // Test 10: Create multiple FSMs in sequence
    suite.addTest("Create multiple FSMs in sequence", []() {
        FSMManager manager;
        
        bool first = manager.createFSM("FirstFSM", "First FSM", std::chrono::milliseconds(100));
        if (!first) {
            return TestFramework::TestResult(false, "Failed to create first FSM", std::chrono::milliseconds(0));
        }
        
        bool second = manager.createFSM("SecondFSM", "Second FSM", std::chrono::milliseconds(200));
        if (!second) {
            return TestFramework::TestResult(false, "Failed to create second FSM", std::chrono::milliseconds(0));
        }
        
        auto fsm = manager.getFSM();
        if (!fsm) {
            return TestFramework::TestResult(false, "Failed to get FSM instance", std::chrono::milliseconds(0));
        }
        
        std::string actualName = fsm->getName();
        return TestFramework::assert_equal(std::string("SecondFSM"), actualName, 
                                        "Manager should contain the last created FSM");
    });
    
    return suite;
}

} // namespace FSMCreationTests

#endif // FSM_CREATION_TESTS_HPP
