#ifndef FSM_CREATION_TESTS_HPP
#define FSM_CREATION_TESTS_HPP

#include "test_framework.hpp"
#include "test_debug_helpers.hpp"
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
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Creation");
        
        FSMManager manager;
        std::string fsmName = "TestFSM";
        std::string fsmDescription = "A test FSM";
        auto stepDelay = std::chrono::milliseconds(100);
        
        debugContext.add("FSM Name", fsmName);
        debugContext.add("FSM Description", fsmDescription);
        debugContext.add("Step Delay (ms)", stepDelay.count());
        
        bool created = manager.createFSM(fsmName, fsmDescription, stepDelay);
        debugContext.add("Creation Result", created);
        
        if (created) {
            // Get FSM to verify it was created
            auto fsm = manager.getFSM();
            bool fsmExists = (fsm != nullptr);
            debugContext.add("FSM Object Exists", fsmExists);
            
            if (fsmExists) {
                std::string actualName = fsm->getName();
                std::string actualDesc = fsm->getDescription();
                auto actualDelay = fsm->getStepDelay();
                
                debugContext.add("Actual Name", actualName);
                debugContext.add("Actual Description", actualDesc);
                debugContext.add("Actual Step Delay (ms)", actualDelay.count());
                
                bool nameMatches = (actualName == fsmName);
                bool descMatches = (actualDesc == fsmDescription);
                bool delayMatches = (actualDelay == stepDelay);
                
                debugContext.add("Name Matches", nameMatches);
                debugContext.add("Description Matches", descMatches);
                debugContext.add("Delay Matches", delayMatches);
            }
        }
        
        return TestFramework::assert_that(created, "FSM should be created successfully", debugContext);
    });
    
    // Test 2: Create FSM with empty name
    suite.addTest("Create FSM with empty name", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Creation with Empty Name");
        
        FSMManager manager;
        std::string fsmName = "";  // Empty name
        std::string fsmDescription = "A test FSM";
        auto stepDelay = std::chrono::milliseconds(100);
        
        debugContext.add("FSM Name", fsmName.empty() ? "(empty)" : fsmName);
        debugContext.add("FSM Description", fsmDescription);
        debugContext.add("Step Delay (ms)", stepDelay.count());
        
        bool created = manager.createFSM(fsmName, fsmDescription, stepDelay);
        debugContext.add("Creation Result", created);
        
        if (created) {
            debugContext.add("Unexpected Result", "FSM was created despite empty name");
            // Get FSM to verify it was unexpectedly created
            auto fsm = manager.getFSM();
            bool fsmExists = (fsm != nullptr);
            debugContext.add("FSM Object Exists", fsmExists);
            
            if (fsmExists) {
                std::string actualName = fsm->getName();
                debugContext.add("Actual Name", actualName.empty() ? "(empty)" : actualName);
            }
        } else {
            debugContext.add("Expected Result", "FSM creation failed due to empty name");
        }
        
        return TestFramework::assert_that(!created, "FSM creation should fail with empty name", debugContext);
    });
    
    // Test 3: Create FSM with very long name
    suite.addTest("Create FSM with very long name", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Creation with Long Name");
        
        FSMManager manager;
        std::string longName(100, 'a'); // Create a string of 100 'a's - much longer than allowed
        std::string fsmDescription = "A test FSM";
        auto stepDelay = std::chrono::milliseconds(100);
        
        debugContext.add("FSM Name Length", longName.length());
        debugContext.add("FSM Name (first 20 chars)", longName.substr(0, 20) + "...");
        debugContext.add("FSM Description", fsmDescription);
        debugContext.add("Step Delay (ms)", stepDelay.count());
        debugContext.add("Expected Max Name Length", 20);  // The implementation limit
        
        bool created = manager.createFSM(longName, fsmDescription, stepDelay);
        debugContext.add("Creation Result", created);
        
        if (created) {
            debugContext.add("Unexpected Result", "FSM was created despite name length exceeding limit");
            // Get FSM to verify it was unexpectedly created
            auto fsm = manager.getFSM();
            bool fsmExists = (fsm != nullptr);
            debugContext.add("FSM Object Exists", fsmExists);
            
            if (fsmExists) {
                std::string actualName = fsm->getName();
                debugContext.add("Actual Name Length", actualName.length());
                debugContext.add("Actual Name", actualName);
                
                // Check if the name was truncated
                bool wasTruncated = actualName.length() < longName.length();
                debugContext.add("Name Was Truncated", wasTruncated);
            }
        } else {
            debugContext.add("Expected Result", "FSM creation failed due to name exceeding length limit");
        }
        
        // The implementation limits names to 20 characters, so this should fail
        return TestFramework::assert_that(!created, "FSM should reject names longer than 20 characters", debugContext);
    });
    
    // Test 4: Create FSM with negative step delay
    suite.addTest("Create FSM with negative step delay", []() {
        FSMManager manager;
        TestDebug::DebugContext debug;
        
        auto delay = std::chrono::milliseconds(-100);
        debug.add("FSM Name", "TestFSM");
        debug.add("FSM Description", "A test FSM");
        debug.add("Step Delay", delay.count());
        
        bool created = manager.createFSM("TestFSM", "A test FSM", delay);
        debug.add("Creation Result", created);
        
        // Implementation-dependent: some systems handle negative values differently
        // Here we assume it fails or resets to a positive value
        // Test requires knowledge of actual expected behavior
        return TestFramework::assert_that(created, "FSM should handle negative delay appropriately", debug);
    });
    
    // Test 5: Create FSM with zero step delay
    suite.addTest("Create FSM with zero step delay", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Creation with Zero Delay");
        
        FSMManager manager;
        std::string fsmName = "TestFSM";
        std::string fsmDescription = "A test FSM";
        auto stepDelay = std::chrono::milliseconds(0);
        
        debugContext.add("FSM Name", fsmName);
        debugContext.add("FSM Description", fsmDescription);
        debugContext.add("Step Delay (ms)", stepDelay.count());
        
        bool created = manager.createFSM(fsmName, fsmDescription, stepDelay);
        debugContext.add("Creation Result", created);
        
        if (created) {
            // Get FSM to verify it was created with zero delay
            auto fsm = manager.getFSM();
            bool fsmExists = (fsm != nullptr);
            debugContext.add("FSM Object Exists", fsmExists);
            
            if (fsmExists) {
                auto actualDelay = fsm->getStepDelay();
                debugContext.add("Actual Step Delay (ms)", actualDelay.count());
                bool delayIsZero = (actualDelay.count() == 0);
                debugContext.add("Delay Is Zero", delayIsZero);
            }
        }
        
        return TestFramework::assert_that(created, "FSM should handle zero delay", debugContext);
    });
    
    // Test 6: Create FSM with very large step delay
    suite.addTest("Create FSM with very large step delay", []() {
        TestDebug::DebugContext debugContext;
        debugContext.addHeader("FSM Creation with Large Delay");
        
        FSMManager manager;
        std::string fsmName = "TestFSM";
        std::string fsmDescription = "A test FSM";
        auto stepDelay = std::chrono::milliseconds(1000000);  // 1000 seconds
        
        debugContext.add("FSM Name", fsmName);
        debugContext.add("FSM Description", fsmDescription);
        debugContext.add("Step Delay (ms)", stepDelay.count());
        
        bool created = manager.createFSM(fsmName, fsmDescription, stepDelay);
        debugContext.add("Creation Result", created);
        
        if (created) {
            // Get FSM to verify it was created with large delay
            auto fsm = manager.getFSM();
            bool fsmExists = (fsm != nullptr);
            debugContext.add("FSM Object Exists", fsmExists);
            
            if (fsmExists) {
                auto actualDelay = fsm->getStepDelay();
                debugContext.add("Actual Step Delay (ms)", actualDelay.count());
                bool delayIsLarge = (actualDelay.count() >= 1000000);
                debugContext.add("Delay Is Large (≥1000000ms)", delayIsLarge);
            }
        }
        
        return TestFramework::assert_that(created, "FSM should handle very large delay", debugContext);
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
