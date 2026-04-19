/**
 * @file test_common.hpp
 * @brief Common definitions and forward declarations for test framework
 * 
 * This file provides shared type definitions and forward declarations used
 * by both the test framework and debug helper components. It breaks the 
 * circular dependency between test_framework.hpp and test_debug_helpers.hpp
 * by defining shared types that both depend on.
 * 
 * Features:
 * - Forward declarations for TestDebug namespace
 * - TestResult structure definition
 * - Forward declarations for assertion functions
 * 
 * @author xvalenk00
 * @date May 2025
 * @version 1.0
 * 
 * @note This file should be included by both test_framework.hpp and test_debug_helpers.hpp
 */

#ifndef TEST_COMMON_HPP
#define TEST_COMMON_HPP

#include <string>
#include <chrono>

// Forward declarations for TestDebug namespace
namespace TestDebug {
    class DebugContext;
}

// Forward declarations for TestFramework namespace
namespace TestFramework {
    /**
     * @class TestResult
     * @brief Represents the result of a single test
     */
    struct TestResult {
        bool passed; ///< Whether the test passed
        std::string message; ///< Additional message/details about the test
        std::chrono::milliseconds duration; ///< Time taken to execute the test

        // Default constructor needed for container usage
        TestResult() 
            : passed(false), message("Uninitialized test result"), duration(std::chrono::milliseconds(0)) {}
            
        // Explicit constructor
        TestResult(bool p, const std::string& msg, std::chrono::milliseconds dur)
            : passed(p), message(msg), duration(dur) {}
    };
    
    // Function declarations for assert functions that will be defined in test_framework.hpp
    TestResult assert_that(bool condition, const std::string& message);
    TestResult assert_that(bool condition, const std::string& message, 
                          const TestDebug::DebugContext& debugContext);
                          
    template<typename T>
    TestResult assert_equal(const T& expected, const T& actual, const std::string& message);
    
    template<typename T>
    TestResult assert_equal(const T& expected, const T& actual, 
                          const std::string& message, const TestDebug::DebugContext& debugContext);
}

#endif // TEST_COMMON_HPP
