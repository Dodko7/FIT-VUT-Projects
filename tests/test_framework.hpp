#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <sstream>
#include <fstream>

/**
 * @brief Output stream operator for vector types
 * Used to print vector contents in test failure messages
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i < vec.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}

/**
 * @file test_framework.hpp
 * @brief A simple test framework for FSM implementation testing
 * 
 * This framework provides functionality to organize, run and report on
 * tests for the FSM implementation.
 */

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
    
    TestResult(bool p, const std::string& msg, std::chrono::milliseconds d)
        : passed(p), message(msg), duration(d) {}
};

/**
 * @class TestSuite
 * @brief Class to manage a collection of related tests
 */
class TestSuite {
private:
    std::string name; ///< Name of the test suite
    std::string description; ///< Description of what's being tested
    std::vector<std::pair<std::string, std::function<TestResult()>>> tests; ///< Tests in this suite
    std::unordered_map<std::string, TestResult> results; ///< Results of executed tests
    bool suiteSetupDone = false; ///< Whether suite setup has been completed

    std::function<bool()> setupFunc = []() { return true; }; ///< Setup function for the suite
    std::function<bool()> teardownFunc = []() { return true; }; ///< Teardown function for the suite

public:
    /**
     * @brief Constructor for TestSuite
     * @param name Name of the test suite
     * @param description Description of what's being tested
     */
    TestSuite(const std::string& name, const std::string& description)
        : name(name), description(description) {}

    /**
     * @brief Set the setup function for the suite
     * @param func Function to be called before any tests are run
     */
    void setSuiteSetup(std::function<bool()> func) {
        setupFunc = func;
    }

    /**
     * @brief Set the teardown function for the suite
     * @param func Function to be called after all tests are run
     */
    void setSuiteTeardown(std::function<bool()> func) {
        teardownFunc = func;
    }

    /**
     * @brief Add a test to the suite
     * @param testName Name of the test
     * @param testFunc Function containing the test logic
     */
    void addTest(const std::string& testName, std::function<TestResult()> testFunc) {
        tests.push_back(std::make_pair(testName, testFunc));
    }

    /**
     * @brief Run all tests in the suite
     * @return True if all tests passed, false otherwise
     */
    bool runAllTests() {
        std::cout << "\n=== Running Test Suite: " << name << " ===\n" << std::endl;
        std::cout << "Description: " << description << "\n" << std::endl;
        
        // Run suite setup once
        if (!suiteSetupDone) {
            std::cout << "Performing suite setup..." << std::endl;
            if (!setupFunc()) {
                std::cout << "Suite setup failed. Aborting tests." << std::endl;
                return false;
            }
            suiteSetupDone = true;
            std::cout << "Suite setup completed successfully.\n" << std::endl;
        }
        
        bool allPassed = true;
        
        for (const auto& test : tests) {
            std::cout << "Running test: " << test.first << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            TestResult result = test.second();
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Update result with actual duration
            TestResult finalResult(result.passed, result.message, duration);
            results[test.first] = finalResult;
            
            if (!result.passed) {
                allPassed = false;
            }
            
            std::cout << "  Result: " << (result.passed ? "PASS" : "FAIL") << std::endl;
            if (!result.message.empty()) {
                std::cout << "  Details: " << result.message << std::endl;
            }
            std::cout << "  Duration: " << duration.count() << "ms\n" << std::endl;
        }
        
        return allPassed;
    }

    /**
     * @brief Generate a report of the test results
     * @return A string containing the formatted report
     */
    std::string generateReport() const {
        std::stringstream report;
        
        // Calculate stats
        int totalTests = results.size();
        int passedTests = 0;
        int failedTests = 0;
        std::chrono::milliseconds totalDuration(0);
        
        for (const auto& result : results) {
            if (result.second.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            totalDuration += result.second.duration;
        }
        
        // Generate report header
        report << "\n=== Test Suite Report: " << name << " ===\n" << std::endl;
        report << "Description: " << description << "\n" << std::endl;
        
        // Generate summary
        report << "SUMMARY:" << std::endl;
        report << "  Total Tests:  " << totalTests << std::endl;
        report << "  Passed Tests: " << passedTests << std::endl;
        report << "  Failed Tests: " << failedTests << std::endl;
        report << "  Total Time:   " << totalDuration.count() << "ms\n" << std::endl;
        
        // Generate detailed results
        report << "DETAILS:" << std::endl;
        for (const auto& result : results) {
            report << "  " << std::setw(30) << std::left << result.first 
                   << ": " << (result.second.passed ? "PASS" : "FAIL") 
                   << " (" << result.second.duration.count() << "ms)" << std::endl;
            
            if (!result.second.message.empty()) {
                report << "      " << result.second.message << std::endl;
            }
        }
        
        report << "\n=== End of Report ===\n" << std::endl;
        
        return report.str();
    }
    
    /**
     * @brief Clean up resources used by the suite
     */
    void cleanup() {
        if (suiteSetupDone) {
            std::cout << "Performing suite teardown..." << std::endl;
            if (!teardownFunc()) {
                std::cout << "Warning: Suite teardown failed." << std::endl;
            } else {
                std::cout << "Suite teardown completed successfully." << std::endl;
            }
            suiteSetupDone = false;
        }
    }

    /**
     * @brief Destroy the TestSuite, ensuring cleanup
     */
    ~TestSuite() {
        cleanup();
    }
    
    /**
     * @brief Get the name of the test suite
     * @return The name of the test suite
     */
    std::string getName() const {
        return name;
    }
    
    /**
     * @brief Get the results of the tests
     * @return Map of test names to results
     */
    const std::unordered_map<std::string, TestResult>& getResults() const {
        return results;
    }
};

/**
 * @class TestRunner
 * @brief Class to manage multiple test suites
 */
class TestRunner {
private:
    std::vector<TestSuite> suites; ///< Test suites to be run

public:
    /**
     * @brief Add a test suite to the runner
     * @param suite The suite to add
     */
    void addSuite(const TestSuite& suite) {
        suites.push_back(suite);
    }

    /**
     * @brief Run all test suites
     * @return True if all suites passed, false otherwise
     */
    bool runAllSuites() {
        std::cout << "\n===============================================" << std::endl;
        std::cout << "           RUNNING ALL TEST SUITES             " << std::endl;
        std::cout << "===============================================\n" << std::endl;
        
        bool allPassed = true;
        
        for (auto& suite : suites) {
            bool suitePassed = suite.runAllTests();
            if (!suitePassed) {
                allPassed = false;
            }
        }
        
        return allPassed;
    }

    /**
     * @brief Generate a report of all test results
     * @param outputFile If provided, write report to this file
     * @return A string containing the formatted report
     */
    std::string generateReport(const std::string& outputFile = "") const {
        std::stringstream report;
        
        // Calculate overall stats
        int totalSuites = suites.size();
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        std::chrono::milliseconds totalDuration(0);
        
        for (const auto& suite : suites) {
            const auto& results = suite.getResults();
            totalTests += results.size();
            
            for (const auto& result : results) {
                if (result.second.passed) {
                    passedTests++;
                } else {
                    failedTests++;
                }
                totalDuration += result.second.duration;
            }
        }
        
        // Generate report header
        report << "\n===============================================" << std::endl;
        report << "                 TEST REPORT                   " << std::endl;
        report << "===============================================\n" << std::endl;
        
        // Generate overall summary
        report << "OVERALL SUMMARY:" << std::endl;
        report << "  Total Suites:  " << totalSuites << std::endl;
        report << "  Total Tests:   " << totalTests << std::endl;
        report << "  Passed Tests:  " << passedTests << std::endl;
        report << "  Failed Tests:  " << failedTests << std::endl;
        report << "  Success Rate:  " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%" << std::endl;
        report << "  Total Time:    " << totalDuration.count() << "ms\n" << std::endl;
        
        // Generate suite summaries
        report << "SUITE SUMMARIES:" << std::endl;
        for (const auto& suite : suites) {
            const auto& results = suite.getResults();
            int suitePassed = 0;
            
            for (const auto& result : results) {
                if (result.second.passed) {
                    suitePassed++;
                }
            }
            
            report << "  " << std::setw(30) << std::left << suite.getName() 
                   << ": " << suitePassed << "/" << results.size() << " passed ("
                   << (results.size() > 0 ? (suitePassed * 100 / results.size()) : 0) << "%)" << std::endl;
        }
        
        report << "\n===============================================" << std::endl;
        report << "                DETAILED RESULTS                " << std::endl;
        report << "===============================================\n" << std::endl;
        
        // Include detailed reports from each suite
        for (const auto& suite : suites) {
            report << suite.generateReport();
        }
        
        report << "\n===============================================" << std::endl;
        report << "                END OF REPORT                   " << std::endl;
        report << "===============================================\n" << std::endl;
        
        // If an output file was provided, write to the file
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << report.str();
                file.close();
                std::cout << "Test report written to: " << outputFile << std::endl;
            } else {
                std::cerr << "Error: Could not open file for writing: " << outputFile << std::endl;
            }
        }
        
        return report.str();
    }
};

/**
 * @brief Assertion function to check a condition and create a test result
 * @param condition The condition to check
 * @param message The message to include with the result
 * @return A TestResult object indicating pass/fail
 */
inline TestResult assert_that(bool condition, const std::string& message = "") {
    if (condition) {
        return TestResult(true, "Assertion passed: " + message, std::chrono::milliseconds(0));
    } else {
        return TestResult(false, "Assertion failed: " + message, std::chrono::milliseconds(0));
    }
}

/**
 * @brief Helper function to compare expected and actual values with custom message
 * @param expected The expected value
 * @param actual The actual value
 * @param message Additional context message
 * @return A TestResult object indicating pass/fail
 */
template<typename T>
inline TestResult assert_equal(const T& expected, const T& actual, const std::string& message = "") {
    std::stringstream detailedMessage;
    bool passed = (expected == actual);
    
    if (passed) {
        detailedMessage << "Values match" << (message.empty() ? "" : ": " + message);
    } else {
        detailedMessage << "Expected: ";
        detailedMessage << expected;
        detailedMessage << ", Actual: ";
        detailedMessage << actual;
        if (!message.empty()) {
            detailedMessage << " (" << message << ")";
        }
    }
    
    return TestResult(passed, detailedMessage.str(), std::chrono::milliseconds(0));
}

/**
 * @brief Helper function to check if an operation throws an exception
 * @param func The function to execute
 * @param message Additional context message
 * @return A TestResult object indicating pass/fail
 */
template<typename ExceptionType = std::exception>
inline TestResult assert_throws(std::function<void()> func, const std::string& message = "") {
    try {
        func();
        return TestResult(false, "Expected exception not thrown" + (message.empty() ? "" : ": " + message), 
                         std::chrono::milliseconds(0));
    } catch (const ExceptionType&) {
        return TestResult(true, "Expected exception thrown correctly" + (message.empty() ? "" : ": " + message), 
                         std::chrono::milliseconds(0));
    } catch (...) {
        return TestResult(false, "Wrong exception type thrown" + (message.empty() ? "" : ": " + message), 
                         std::chrono::milliseconds(0));
    }
}

/**
 * @brief Helper function to check if an operation doesn't throw an exception
 * @param func The function to execute
 * @param message Additional context message
 * @return A TestResult object indicating pass/fail
 */
inline TestResult assert_no_throw(std::function<void()> func, const std::string& message = "") {
    try {
        func();
        return TestResult(true, "No exception thrown as expected" + (message.empty() ? "" : ": " + message), 
                         std::chrono::milliseconds(0));
    } catch (const std::exception& e) {
        return TestResult(false, "Unexpected exception thrown: " + std::string(e.what()) + 
                         (message.empty() ? "" : " (" + message + ")"), 
                         std::chrono::milliseconds(0));
    } catch (...) {
        return TestResult(false, "Unexpected non-standard exception thrown" + 
                         (message.empty() ? "" : " (" + message + ")"), 
                         std::chrono::milliseconds(0));
    }
}

} // namespace TestFramework

#endif // TEST_FRAMEWORK_HPP