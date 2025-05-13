#include "test_framework.hpp"
#include "fsm_creation_tests.hpp"
#include "fsm_state_management_tests.hpp"
#include "fsm_file_operations_tests.hpp"
#include "fsm_transition_management_tests.hpp"
#include "fsm_variable_input_tests.hpp"
#include "fsm_execution_tests.hpp"
#include "test_output_formatter.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <QCoreApplication>
#include <QScriptEngine>
#include <fstream>
#include <sstream>
#include <iomanip>

/**
 * @file fsm_test_runner.cpp
 * @brief Main test runner for FSM implementation tests
 * 
 * This file runs all test suites for the FSM implementation and 
 * generates a comprehensive report of the test results with improved formatting.
 */

// Helper class to redirect cout during tests
class OutputRedirector {
private:
    std::streambuf* originalCoutBuffer;
    std::ofstream devNull;
    bool redirecting;

public:
    OutputRedirector() : redirecting(false) {
        originalCoutBuffer = std::cout.rdbuf();
    }

    // Redirect to /dev/null or NUL on Windows
    void suppressOutput() {
        if (!redirecting) {
            #ifdef _WIN32
            devNull.open("NUL");
            #else
            devNull.open("/dev/null");
            #endif
            std::cout.rdbuf(devNull.rdbuf());
            redirecting = true;
        }
    }

    // Restore original output
    void restoreOutput() {
        if (redirecting) {
            std::cout.rdbuf(originalCoutBuffer);
            if (devNull.is_open()) {
                devNull.close();
            }
            redirecting = false;
        }
    }

    ~OutputRedirector() {
        restoreOutput();
    }
};

// Custom report generator that uses improved formatting for the test report
std::string generateFormattedReport(const TestFramework::TestRunner& runner, const std::string& outputFile = "") {
    // Calculate overall stats
    int totalSuites = 0;
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    std::chrono::milliseconds totalDuration(0);
    
    std::vector<std::tuple<std::string, std::string, std::string, TestDebug::DebugContext>> failedTestDetails;
    
    // Get stats and failed test details from all suites
    for (const auto& suite : runner.getSuites()) {
        totalSuites++;
        const auto& results = suite.getResults();
        totalTests += results.size();
        
        for (const auto& result : results) {
            if (result.second.passed) {
                passedTests++;
            } else {
                failedTests++;
                
                // Extract failed test details including debug context
                std::string errorMessage = result.second.message;
                TestDebug::DebugContext debugContext;
                
                // Extract debug context from error message if present
                size_t contextPos = errorMessage.find("--- Debug Context ---");
                if (contextPos != std::string::npos) {
                    std::string contextStr = errorMessage.substr(contextPos);
                    errorMessage = errorMessage.substr(0, contextPos);
                    
                    // Clean up error message - remove trailing whitespace
                    while (!errorMessage.empty() && std::isspace(errorMessage.back())) {
                        errorMessage.pop_back();
                    }
                    
                    // Parse debug context from string
                    debugContext.parseFromString(contextStr);
                }
                
                // Clean up error message - remove "Assertion failed: " prefix if present
                if (errorMessage.substr(0, 17) == "Assertion failed: ") {
                    errorMessage = errorMessage.substr(17);
                }
                
                // Add to failed test details
                failedTestDetails.push_back(std::make_tuple(
                    suite.getName(),
                    result.first,
                    errorMessage,
                    debugContext
                ));
            }
            totalDuration += result.second.duration;
        }
    }
    
    // Generate summary string
    std::stringstream summary;
    summary << "  Total Suites:  " << totalSuites << std::endl;
    summary << "  Total Tests:   " << totalTests << std::endl;
    summary << "  Passed Tests:  " << passedTests << std::endl;
    summary << "  Failed Tests:  " << failedTests << std::endl;
    summary << "  Success Rate:  " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%" << std::endl;
    summary << "  Total Time:    " << totalDuration.count() << "ms";
    
    // Generate suite summaries string
    std::stringstream suiteSummaries;
    for (const auto& suite : runner.getSuites()) {
        const auto& results = suite.getResults();
        int suitePassed = 0;
        
        for (const auto& result : results) {
            if (result.second.passed) {
                suitePassed++;
            }
        }
        
        suiteSummaries << "  " << std::setw(30) << std::left << suite.getName() 
                << ": " << suitePassed << "/" << results.size() << " passed ("
                << (results.size() > 0 ? (suitePassed * 100 / results.size()) : 0) << "%)" << std::endl;
    }
    
    // Get detailed results from each suite
    std::stringstream detailedResults;
    for (const auto& suite : runner.getSuites()) {
        detailedResults << suite.generateReport();
    }
    
    // Format and write the report
    std::string formattedReport = TestOutputFormatter::formatTestReport(
        summary.str(),
        failedTestDetails,
        suiteSummaries.str(),
        detailedResults.str()
    );
    
    // Write to file if specified
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << formattedReport;
            file.close();
            std::cout << "Test report written to: " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Could not open file for writing: " << outputFile << std::endl;
        }
    }
    
    return formattedReport;
}

// Helper function to print nicely formatted section headers
void printBanner(const std::string& message) {
    int width = 70;
    std::string line(width, '=');
    std::string spaces((width - message.length()) / 2 - 1, ' ');
    
    std::cout << "\n+" << line << "+\n";
    std::cout << "| " << spaces << message << spaces;
    // Adjust if odd length
    if ((message.length() % 2) != (width % 2)) std::cout << " ";
    std::cout << " |\n";
    std::cout << "+" << line << "+\n";
}

int main(int argc, char* argv[]) {
    printBanner("FSM Implementation Test Runner");
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();

    // Initialize QCoreApplication for QScriptEngine support
    QCoreApplication app(argc, argv);
    
    // Create output redirector to suppress FSM output during tests
    OutputRedirector redirector;
    
    // Parse command line arguments for verbose mode
    bool verbose = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
            break;
        }
    }
    
    // Only suppress output if not in verbose mode
    if (!verbose) {
        std::cout << "Running in quiet mode. Use --verbose or -v for detailed FSM output." << std::endl;
        redirector.suppressOutput();
    }
    
    // Known issues list for better error reporting
    std::cout << "Note: The following tests have known issues that may cause failures:" << std::endl;
    std::cout << "  1. FSM Execution Tests::Reset FSM - FSM has no current state" << std::endl;
    std::cout << "  2. FSM Execution Tests::Step through FSM - Steps may report failure but sequence works" << std::endl; 
    std::cout << "  3. FSM Transition Management Tests::Remove a transition - Transition verification needs improvement" << std::endl;
    std::cout << std::endl;
    
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
    
    // Restore output for test results
    redirector.restoreOutput();
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Generate report using our custom formatter
    std::string reportFilePath = "fsm_test_report.txt";
    std::string report = generateFormattedReport(runner, reportFilePath);
    
    // Print final summary to console
    printBanner("TEST SUMMARY");
    
    std::cout << "Total Test Duration: " << duration.count() << " seconds" << std::endl;
    
    if (allPassed) {
        std::cout << "Overall Result: \033[1;32mPASSED\033[0m" << std::endl;
    } else {
        std::cout << "Overall Result: \033[1;31mFAILED\033[0m" << std::endl;
        std::cout << "\033[1;33mCheck the detailed report for information about failed tests.\033[0m" << std::endl;
        std::cout << "\033[1;33mSome failures are part of known issues listed above.\033[0m" << std::endl;
    }
    
    std::cout << "Detailed report written to: " << reportFilePath << std::endl;
    
    // Print machine-readable result line for CI integration
    std::cout << "TEST_RESULT:" << (allPassed ? "PASS" : "FAIL") << std::endl;
    
    return allPassed ? 0 : 1;
}
