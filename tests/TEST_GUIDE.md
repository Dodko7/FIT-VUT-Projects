# Finite State Machine Test Framework Guide

This document explains how to use the FSM Test Framework to create new tests, organize test suites, and understand test results.

## Table of Contents

1. [Introduction](#introduction)
2. [Framework Overview](#framework-overview)
3. [Creating a New Test](#creating-a-new-test)
4. [Creating a Test Suite](#creating-a-test-suite)
5. [Running Tests](#running-tests)
6. [Test Assertions](#test-assertions)
7. [Understanding Test Output](#understanding-test-output)
8. [Known Issues](#known-issues)
9. [Best Practices](#best-practices)

## Introduction

The FSM Testing Framework is designed to help test the Finite State Machine implementation with a structured, maintainable approach. It provides tools for organizing tests, capturing detailed debug information, and producing comprehensive test reports.

## Framework Overview

The framework consists of several key components:

1. **Test Framework** (`test_framework.hpp`): Provides the core testing infrastructure, including test suites, test execution, and assertion functions.
2. **Debug Helpers** (`test_debug_helpers.hpp`): Tools for capturing and displaying state information when tests fail.
3. **Output Formatter** (`test_output_formatter.hpp`): Enhanced formatting for test reports and debug output.

## Creating a New Test

Tests are defined using lambda functions that are added to a test suite. To create a new test:

```cpp
suite.addTest("Test Name", []() {
    // Test setup and logic
    FSMManager manager;
    bool result = manager.createFSM("TestFSM", "Description", std::chrono::milliseconds(100));
    
    // Return a test result using an assertion function
    return TestFramework::assert_that(result, "FSM should be created successfully");
});
```

### Using Debug Context

For more complex tests, use `DebugContext` to capture detailed information about the test execution:

```cpp
suite.addTest("Test with Debug Context", []() {
    TestDebug::DebugContext debugContext;
    debugContext.addHeader("FSM Setup");
    
    FSMManager manager;
    bool createResult = manager.createFSM("TestFSM", "A test FSM", std::chrono::milliseconds(100));
    debugContext.add("FSM Creation Result", createResult);
    
    if (!createResult) {
        return TestFramework::assert_that(false, "Failed to create FSM", debugContext);
    }
    
    // Organize debug information into sections
    debugContext.addHeader("State Addition");
    bool stateAdded = manager.addState("State1", "code", 'A', false);
    debugContext.add("State Added", stateAdded);
    
    return TestFramework::assert_that(stateAdded, "State should be added successfully", debugContext);
});
```

## Creating a Test Suite

A test suite is a collection of related tests. Each test suite can have its own setup and teardown functions:

```cpp
TestFramework::TestSuite createTestSuite() {
    TestFramework::TestSuite suite("Suite Name", "Description of the test suite");
    
    // Add setup function (runs once before any tests in the suite)
    suite.setSuiteSetup([]() {
        std::cout << "Setting up test suite..." << std::endl;
        // Initialize resources...
        return true; // Return false to abort the suite
    });
    
    // Add teardown function (runs once after all tests in the suite)
    suite.setSuiteTeardown([]() {
        std::cout << "Tearing down test suite..." << std::endl;
        // Clean up resources...
        return true;
    });
    
    // Add tests to the suite
    suite.addTest("Test 1", []() {
        // Test implementation...
        return TestFramework::assert_that(true, "Test message");
    });
    
    suite.addTest("Test 2", []() {
        // Test implementation...
        return TestFramework::assert_that(true, "Test message");
    });
    
    return suite;
}
```

## Running Tests

Tests are run using the `TestRunner` class in the main test file. The current implementation includes support for suppressing debug output and specifying verbosity:

```cpp
int main(int argc, char* argv[]) {
    // Create test runner
    TestFramework::TestRunner runner;
    
    // Add test suites
    runner.addSuite(FSMCreationTests::createTestSuite());
    runner.addSuite(FSMStateManagementTests::createTestSuite());
    // ...add other test suites
    
    // Run all suites
    bool allPassed = runner.runAllSuites();
    
    // Generate a report
    std::string reportFile = "fsm_test_report.txt";
    runner.generateReport(reportFile);
    
    return allPassed ? 0 : 1;
}
```

To run the tests with verbose output (showing FSM operations):

```bash
./fsm_test_runner --verbose
```

## Test Assertions

The framework provides several assertion functions:

1. **assert_that(condition, message, [debugContext])**: Verifies a boolean condition
   ```cpp
   return TestFramework::assert_that(manager.validateFSM(), "FSM should be valid");
   ```

2. **assert_equal(expected, actual, message, [debugContext])**: Compares two values for equality
   ```cpp
   return TestFramework::assert_equal(3, states.size(), "Should have 3 states");
   ```

3. **assert_throws<ExceptionType>(func, message, [debugContext])**: Checks if code throws the expected exception
   ```cpp
   return TestFramework::assert_throws<std::invalid_argument>(
       [&]() { manager.getState("NonExistentState"); },
       "Should throw for non-existent state"
   );
   ```

4. **assert_no_throw(func, message, [debugContext])**: Checks if code doesn't throw exceptions
   ```cpp
   return TestFramework::assert_no_throw(
       [&]() { manager.reset(); },
       "Reset should not throw exceptions"
   );
   ```

## Understanding Test Output

The test runner generates two types of output:

1. **Console Output**: Provides real-time feedback when running tests
2. **Test Report File**: A detailed report saved to `fsm_test_report.txt`

### Console Output Format

```
=== Running Test Suite: Suite Name ===

Description: Suite description

Running test: Test Name
  Result: PASS
  Duration: 5ms

Running test: Failed Test
  Result: FAIL
  Details: Assertion failed: Expected behavior
  Duration: 2ms
```

### Test Report Format

The test report includes:

1. **Overall Summary**: Total tests, passed tests, failed tests, success rate
2. **Failed Tests Summary**: Detailed information about any failed tests
3. **Suite Summaries**: Statistics for each test suite
4. **Detailed Results**: Comprehensive results for each test

### Debug Context Format

Failed tests include a structured debug context that helps diagnose issues:

```
╔══════════════════════════ DEBUG CONTEXT ═══════════════════════════╗
╠══════════════════════════ FSM Setup ═══════════════════════════════╣
║ Setup Success             │ 1                                     ║
╠══════════════════════════ State Addition ═══════════════════════════╣
║ From State                │ State1                                ║
║ To State                  │ State2                                ║
║ Condition                 │ true                                  ║
║ Input Character           │ 1                                     ║
╚═════════════════════════════════════════════════════════════════════╝
```

## Known Issues

The current implementation has a few known issues that might cause test failures:

1. **FSM Execution Tests::Reset FSM** - FSM has no current state
2. **FSM Execution Tests::Step through FSM** - Steps may report failure but sequence works
3. **FSM Transition Management Tests::Remove a transition** - Transition verification needs improvement

These issues will be displayed when running the test runner and are being tracked for future fixes.

## Best Practices

1. **Use Debug Contexts**: Always include detailed debug contexts for comprehensive failure information.

2. **Test One Aspect Per Test**: Each test should focus on a single aspect of functionality.

3. **Organize Related Tests**: Group related tests into the same test suite.

4. **Check Preconditions**: Verify setup steps succeeded before continuing with test logic.

5. **Descriptive Messages**: Use clear, descriptive messages in assertion functions.

6. **Structure Debug Information**: Use `debugContext.addHeader()` to organize debug information into logical sections.

7. **Add Key State Values**: Include all relevant state values in the debug context.

8. **Handle Setup Failures**: Return early with a clear failure message if setup fails.

Example of a well-structured test:

```cpp
suite.addTest("Add and remove a state", []() {
    TestDebug::DebugContext debugContext;
    debugContext.addHeader("FSM Setup");
    
    FSMManager manager;
    bool created = manager.createFSM("TestFSM", "Test FSM", std::chrono::milliseconds(100));
    debugContext.add("FSM Created", created);
    
    if (!created) {
        return TestFramework::assert_that(false, "Failed to create FSM", debugContext);
    }
    
    debugContext.addHeader("State Addition");
    std::string stateName = "TestState";
    bool added = manager.addState(stateName, "code", 'A', false);
    debugContext.add("State Added", added);
    
    if (!added) {
        return TestFramework::assert_that(false, "Failed to add state", debugContext);
    }
    
    debugContext.addHeader("State Removal");
    bool removed = manager.removeState(stateName);
    debugContext.add("State Removed", removed);
    
    auto states = manager.getAllStateNames();
    bool stateGone = std::find(states.begin(), states.end(), stateName) == states.end();
    debugContext.add("State No Longer Exists", stateGone);
    
    return TestFramework::assert_that(removed && stateGone, 
                                     "State should be successfully removed",
                                     debugContext);
});
```

By following these guidelines, you can create effective, maintainable tests that provide clear information when failures occur.
