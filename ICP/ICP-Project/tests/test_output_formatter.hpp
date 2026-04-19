#ifndef TEST_OUTPUT_FORMATTER_HPP
#define TEST_OUTPUT_FORMATTER_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include "test_debug_helpers.hpp"

/**
 * @file test_output_formatter.hpp
 * @brief Utilities for formatting test output in a more readable way
 */

namespace TestOutputFormatter {

/**
 * @brief Format debug context with improved styling for text reports
 * @param debugContext The debug context to format
 * @return Formatted string with better formatting for readability
 */
inline std::string formatDebugContext(const TestDebug::DebugContext& debugContext) {
    std::stringstream result;
    std::string debugStr = debugContext.toString();
    
    // Split the debug string into lines
    std::istringstream stream(debugStr);
    std::string line;
    std::string currentHeader;
    
    // First, check if the debug context is not empty
    if (debugStr.find("--- Debug Context ---") == std::string::npos) {
        return "(No debug context available)";
    }
    
    // Skip the initial "--- Debug Context ---" line as we'll reformat it
    while (std::getline(stream, line) && line.find("--- Debug Context ---") != std::string::npos) {
        // Skip this line
    }
    
    // Add our own better formatted header
    result << "\n+---------------------------- DEBUG CONTEXT ----------------------------+\n";
    
    // Process the rest of the debug context
    bool inHeader = false;
    
    do {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Check if this is a header line
        if (line.find("##") != std::string::npos) {
            // Extract header text
            currentHeader = line;
            size_t start = line.find("##") + 2;
            size_t end = line.rfind("##");
            
            if (end != std::string::npos && end > start) {
                currentHeader = line.substr(start, end - start);
            } else {
                currentHeader = line.substr(start);
            }
            
            // Trim whitespace
            currentHeader.erase(0, currentHeader.find_first_not_of(" \t"));
            currentHeader.erase(currentHeader.find_last_not_of(" \t") + 1);
            
            result << "+---------------------------- " << currentHeader << " ";
            // Fill the rest of the line with -
            int fillChars = 28 - currentHeader.length();
            if (fillChars > 0) {
                result << std::string(fillChars, '-');
            }
            result << "+\n";
            inHeader = true;
        } 
        // Skip separator lines
        else if (line.find("----") != std::string::npos) {
            // Skip separator line
            continue;
        }
        // Regular key-value pair
        else if (!line.empty() && inHeader) {
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Format as a nice table row
                result << "| " << std::setw(30) << std::left << key 
                       << " | " << std::setw(35) << std::left << value << " |\n";
            } else {
                // Lines without colon are treated as messages
                result << "| " << std::setw(67) << std::left << line << " |\n";
            }
        }
    } while (std::getline(stream, line));
    
    result << "+-----------------------------------------------------------------------+\n";
    
    return result.str();
}

/**
 * @brief Format test error message with improved styling
 * @param testName Name of the test
 * @param errorMessage Error message to format
 * @param debugContext Optional debug context
 * @return Formatted error string
 */
inline std::string formatTestError(const std::string& testName, 
                                  const std::string& errorMessage,
                                  const TestDebug::DebugContext* debugContext = nullptr) {
    std::stringstream result;
    
    // Add error message with simple formatting
    result << "Assertion failed: " << errorMessage;
    
    // Add debug context if provided
    if (debugContext != nullptr) {
        result << "\n" << formatDebugContext(*debugContext);
    }
    
    return result.str();
}

/**
 * @brief Format a test report file with improved readability
 * @param summary Overall test summary
 * @param failedTests List of failed tests with details
 * @param suiteSummaries Summary for each test suite
 * @param detailedResults Detailed results for each test
 * @return Formatted report string
 */
inline std::string formatTestReport(
    const std::string& summary,
    const std::vector<std::tuple<std::string, std::string, std::string, TestDebug::DebugContext>>& failedTests,
    const std::string& suiteSummaries,
    const std::string& detailedResults
) {
    std::stringstream report;
    
    // Report header
    report << "\n+===============================================================+\n";
    report << "|                         TEST REPORT                           |\n";
    report << "+===============================================================+\n\n";
    
    // Overall summary
    report << "OVERALL SUMMARY:\n";
    report << summary << "\n";
    
    // Failed tests summary with improved formatting
    if (!failedTests.empty()) {
        report << "FAILED TESTS SUMMARY:\n";
        
        for (size_t i = 0; i < failedTests.size(); ++i) {
            const auto& [suiteName, testName, errorMessage, debugContext] = failedTests[i];
            report << "  " << (i+1) << ". " << suiteName << "::" << testName << "\n";
            report << "     Assertion failed: " << errorMessage << "\n\n";
            
            // Format debug context with proper indentation
            std::string contextStr = formatDebugContext(debugContext);
            std::istringstream contextStream(contextStr);
            std::string line;
            
            while (std::getline(contextStream, line)) {
                if (!line.empty()) {
                    // Don't indent the section headers starting with +
                    if (line[0] == '+') {
                        report << "     " << line << "\n";
                    } else {
                        report << "     " << line << "\n";
                    }
                }
            }
            
            // Add extra spacing between failed tests
            if (i < failedTests.size() - 1) {
                report << "\n";
            }
        }
    }
    
    // Suite summaries
    report << "\nSUITE SUMMARIES:\n";
    report << suiteSummaries << "\n";
    
    // Detailed results
    report << "+===============================================================+\n";
    report << "|                      DETAILED RESULTS                         |\n";
    report << "+===============================================================+\n\n";
    report << detailedResults << "\n";
    
    // Footer
    report << "+===============================================================+\n";
    report << "|                       END OF REPORT                           |\n";
    report << "+===============================================================+\n";
    
    return report.str();
}

} // namespace TestOutputFormatter

#endif // TEST_OUTPUT_FORMATTER_HPP
