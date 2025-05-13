#ifndef TEST_OUTPUT_UTILS_HPP
#define TEST_OUTPUT_UTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

/**
 * @file test_output_utils.hpp
 * @brief Utilities for managing output during tests
 * 
 * This file provides utilities to redirect, suppress, or capture
 * stdout and stderr during test execution for cleaner test output.
 */

namespace TestUtils {

/**
 * @class OutputCapture
 * @brief Utility class to capture stdout during test execution
 */
class OutputCapture {
private:
    std::stringstream buffer;
    std::streambuf* oldCoutBuffer;
    bool capturing;

public:
    OutputCapture() : capturing(false), oldCoutBuffer(nullptr) {}

    /**
     * @brief Start capturing stdout to an internal buffer
     */
    void start() {
        if (!capturing) {
            oldCoutBuffer = std::cout.rdbuf();
            std::cout.rdbuf(buffer.rdbuf());
            capturing = true;
        }
    }

    /**
     * @brief Stop capturing and restore normal stdout
     */
    void stop() {
        if (capturing) {
            std::cout.rdbuf(oldCoutBuffer);
            capturing = false;
        }
    }

    /**
     * @brief Get the captured output
     * @return String containing the captured output
     */
    std::string getOutput() const {
        return buffer.str();
    }

    /**
     * @brief Clear the captured output buffer
     */
    void clear() {
        buffer.str("");
        buffer.clear();
    }

    /**
     * @brief Destructor ensures stdout is restored
     */
    ~OutputCapture() {
        stop();
    }
};

/**
 * @class OutputSuppressor
 * @brief Utility class to suppress stdout during test execution
 */
class OutputSuppressor {
private:
    std::ofstream nullStream;
    std::streambuf* oldCoutBuffer;
    bool suppressing;

public:
    OutputSuppressor() : suppressing(false), oldCoutBuffer(nullptr) {}

    /**
     * @brief Start suppressing stdout (redirect to null)
     */
    void start() {
        if (!suppressing) {
            oldCoutBuffer = std::cout.rdbuf();
            #ifdef _WIN32
            nullStream.open("NUL");
            #else
            nullStream.open("/dev/null");
            #endif
            std::cout.rdbuf(nullStream.rdbuf());
            suppressing = true;
        }
    }

    /**
     * @brief Stop suppressing and restore normal stdout
     */
    void stop() {
        if (suppressing) {
            std::cout.rdbuf(oldCoutBuffer);
            nullStream.close();
            suppressing = false;
        }
    }

    /**
     * @brief Destructor ensures stdout is restored
     */
    ~OutputSuppressor() {
        stop();
    }
};

/**
 * @brief Global function to run a function with suppressed output
 * @param func The function to execute with suppressed output
 */
template<typename Func>
void runWithSuppressedOutput(Func func) {
    OutputSuppressor suppressor;
    suppressor.start();
    try {
        func();
    } catch (...) {
        suppressor.stop();
        throw;
    }
    suppressor.stop();
}

/**
 * @brief Global function to run a function with captured output
 * @param func The function to execute with captured output
 * @return String containing the captured output
 */
template<typename Func>
std::string captureOutput(Func func) {
    OutputCapture capture;
    capture.start();
    try {
        func();
    } catch (...) {
        capture.stop();
        throw;
    }
    capture.stop();
    return capture.getOutput();
}

} // namespace TestUtils

#endif // TEST_OUTPUT_UTILS_HPP
