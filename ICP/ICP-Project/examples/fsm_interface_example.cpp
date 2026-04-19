#include "fsmInterface.hpp"
#include <iostream>
#include <QtCore/QCoreApplication>
#include <fstream>
#include <algorithm>
#include <set>

/**
 * @file fsm_interface_example.cpp
 * @brief Example demonstrating how to use the FSMManager interface class
 * 
 * This example shows the basic usage of the FSMManager class to create, 
 * manipulate, and execute a simple Finite State Machine.
 */

// Function to compare two FSM instances to check if they are equivalent
bool compareFSMs(FSMManager& fsm1, FSMManager& fsm2) {
    std::cout << "Comparing FSMs..." << std::endl;
    
    // Compare basic properties
    auto states1 = fsm1.getAllStateNames();
    auto states2 = fsm2.getAllStateNames();
    auto inputs1 = fsm1.getExpectedInputs();
    auto inputs2 = fsm2.getExpectedInputs();
    
    // Sort states and inputs for comparison
    std::sort(states1.begin(), states1.end());
    std::sort(states2.begin(), states2.end());
    std::sort(inputs1.begin(), inputs1.end());
    std::sort(inputs2.begin(), inputs2.end());
    
    // Compare state count
    if (states1.size() != states2.size()) {
        std::cout << "State count mismatch: " << states1.size() << " vs " << states2.size() << std::endl;
        return false;
    }
    
    // Compare expected inputs
    if (inputs1.size() != inputs2.size()) {
        std::cout << "Expected inputs count mismatch: " << inputs1.size() << " vs " << inputs2.size() << std::endl;
        return false;
    }
    
    // Compare state names
    if (!std::equal(states1.begin(), states1.end(), states2.begin())) {
        std::cout << "State names mismatch" << std::endl;
        return false;
    }
    
    // Compare expected inputs
    if (!std::equal(inputs1.begin(), inputs1.end(), inputs2.begin())) {
        std::cout << "Expected inputs mismatch" << std::endl;
        return false;
    }
    
    // Compare details of each state
    for (const auto& stateName : states1) {
        auto stateInfo1 = fsm1.getStateInfo(stateName);
        auto stateInfo2 = fsm2.getStateInfo(stateName);
        
        if (stateInfo1["output"] != stateInfo2["output"]) {
            std::cout << "State output mismatch for state " << stateName << std::endl;
            return false;
        }
        
        if (stateInfo1["isFinal"] != stateInfo2["isFinal"]) {
            std::cout << "Final state flag mismatch for state " << stateName << std::endl;
            return false;
        }
        
        // Compare transitions for this state
        auto transitions1 = fsm1.getStateTransitions(stateName);
        auto transitions2 = fsm2.getStateTransitions(stateName);
        
        if (transitions1.size() != transitions2.size()) {
            std::cout << "Transition count mismatch for state " << stateName << std::endl;
            return false;
        }
        
        // Create sets of transition targets for comparison
        std::set<std::string> targets1, targets2;
        std::set<char> inputs1, inputs2;
        
        for (const auto& transition : transitions1) {
            targets1.insert(transition["toState"].get<std::string>());
            inputs1.insert(transition["input"].get<std::string>()[0]);
        }
        
        for (const auto& transition : transitions2) {
            targets2.insert(transition["toState"].get<std::string>());
            inputs2.insert(transition["input"].get<std::string>()[0]);
        }
        
        if (targets1 != targets2) {
            std::cout << "Transition targets mismatch for state " << stateName << std::endl;
            return false;
        }
        
        if (inputs1 != inputs2) {
            std::cout << "Transition inputs mismatch for state " << stateName << std::endl;
            return false;
        }
    }
    
    std::cout << "FSMs are equivalent!" << std::endl;
    return true;
}

int main(int argc, char *argv[]) {
    // Initialize QCoreApplication for QScriptEngine support
    QCoreApplication app(argc, argv);
    
    // Create a new FSM interface instance
    std::cout << "\n=== Creating a new FSM interface instance ===\n" << std::endl;
    FSMManager manager;
    
    // Create a new FSM
    std::cout << "\n=== Creating a new FSM ===\n" << std::endl;
    if (!manager.createFSM("BinaryCounter", "A simple binary counter FSM", std::chrono::milliseconds(100))) {
        std::cerr << "Error creating FSM: " << manager.getLastError() << std::endl;
        return 1;
    }

    
    // Add states
    std::cout << "\n=== Adding states ===\n" << std::endl;
    manager.addState("Init", "", '0', false);
    manager.addState("State0", "", '0', false);
    manager.addState("State1", "", '1', false);
    manager.addState("EndState", "", 'X', true);
    
    // Set start state
    std::cout << "\n=== Setting start state ===\n" << std::endl;
    manager.setStartState("Init");
    
    // Add transitions
    std::cout << "\n=== Adding transitions ===\n" << std::endl;
    manager.addTransition("Init", "State0", "", '0');
    manager.addTransition("State0", "State1", "", '1');
    manager.addTransition("State1", "State0", "", '0');
    manager.addTransition("State0", "EndState", "", 'e');
    manager.addTransition("State1", "EndState", "", 'e');
    
    // Add expected inputs
    std::cout << "\n=== Adding expected inputs ===\n" << std::endl;
    manager.addExpectedInput('0');
    manager.addExpectedInput('1');
    manager.addExpectedInput('e');
    
    // Test FSM validate
    std::cout << "\n=== Testing FSM validation ===\n" << std::endl;
    manager.validateFSM();
    
    // Test reset without loading
    std::cout << "\n=== Testing reset without loading ===\n" << std::endl;
    manager.reset();
    
    // Test debug stepping
    std::cout << "\n=== Testing debug stepping ===\n" << std::endl;
    manager.setInput("010");
    
    std::cout << "Taking three steps through FSM execution:" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "\n--- Step " << (i+1) << " ---" << std::endl;
        if (manager.step()) {
            std::cout << manager.getCurrentState() << std::endl;
            std::cout << manager.getOutput() << std::endl;
        } else {
            std::cout << "Step ended execution or reached final state" << std::endl;
            break;
        }
    }
    
    // Reset again
    std::cout << "\n=== Resetting FSM ===\n" << std::endl;
    manager.reset();
    
    // Run the full FSM
    std::cout << "\n=== Running the full FSM ===\n" << std::endl;
    manager.setInput("01010e");
    
    // Run the FSM
    if (!manager.run()) {
        std::cerr << "Error running FSM: " << manager.getLastError() << std::endl;
        return 1;
    }
    
    // Print the output
    std::cout << "Final FSM Output: " << manager.getOutput() << std::endl;
    
    // Save the FSM to a file
    std::cout << "\n=== Saving the FSM to a file ===\n" << std::endl;
    if (!manager.saveFSM("binary_counter.json")) {
        std::cerr << "Error saving FSM: " << manager.getLastError() << std::endl;
        return 1;
    }
    std::cout << "FSM saved to binary_counter.json successfully" << std::endl;
    
    // Export the original FSM to JSON string for comparison
    std::cout << "\n=== Exporting original FSM to JSON ===\n" << std::endl;
    std::string originalFsmJson = manager.exportFSMToJson();
    if (originalFsmJson.empty()) {
        std::cerr << "Error exporting FSM to JSON: " << manager.getLastError() << std::endl;
        return 1;
    }
    std::cout << "Original FSM exported successfully" << std::endl;
    
    // Create a new manager and load the FSM
    std::cout << "\n=== Creating a new manager and loading the FSM ===\n" << std::endl;
    FSMManager newManager;
    if (!newManager.loadFSM("binary_counter.json")) {
        std::cerr << "Error loading FSM: " << newManager.getLastError() << std::endl;
        return 1;
    }
    std::cout << "FSM loaded successfully" << std::endl;
    
    // Export the loaded FSM to JSON for comparison
    std::cout << "\n=== Exporting loaded FSM to JSON ===\n" << std::endl;
    std::string loadedFsmJson = newManager.exportFSMToJson();
    if (loadedFsmJson.empty()) {
        std::cerr << "Error exporting loaded FSM to JSON: " << newManager.getLastError() << std::endl;
        return 1;
    }
    std::cout << "Loaded FSM exported successfully" << std::endl;
    
    // Compare JSON strings for exact match
    std::cout << "\n=== Comparing JSON strings ===\n" << std::endl;
    if (originalFsmJson == loadedFsmJson) {
        std::cout << "JSON comparison: Exact match!" << std::endl;
    } else {
        std::cout << "JSON comparison: Different" << std::endl;
        
        // Create JSON objects to compare structure
        try {
            nlohmann::json originalJson = nlohmann::json::parse(originalFsmJson);
            nlohmann::json loadedJson = nlohmann::json::parse(loadedFsmJson);
            
            // Check if the structures match (ignoring formatting differences)
            if (originalJson == loadedJson) {
                std::cout << "However, JSON structures are equivalent (formatting differences only)" << std::endl;
            } else {
                std::cout << "JSON structures are different" << std::endl;
            }
        } catch(const std::exception& e) {
            std::cerr << "Error comparing JSON: " << e.what() << std::endl;
        }
    }
    
    // Compare FSM objects directly
    std::cout << "\n=== Comparing FSM objects ===\n" << std::endl;
    bool fsmsEqual = compareFSMs(manager, newManager);
    std::cout << "FSM comparison result: " << (fsmsEqual ? "Equivalent" : "Different") << std::endl;
    
    // Functional test - compare execution of both FSMs with the same input
    std::cout << "\n=== Functional test - Running both FSMs with the same input ===\n" << std::endl;
    
    // Prepare a test input
    const std::string testInput = "01010e";
    
    // Reset both FSMs
    manager.reset();
    newManager.reset();
    
    // Run original FSM
    std::cout << "Running original FSM with input: " << testInput << std::endl;
    manager.setInput(testInput);
    if (!manager.run()) {
        std::cerr << "Error running original FSM: " << manager.getLastError() << std::endl;
    }
    std::string originalOutput = manager.getOutput();
    std::cout << "Original FSM output: " << originalOutput << std::endl;
    
    // Run loaded FSM
    std::cout << "Running loaded FSM with input: " << testInput << std::endl;
    newManager.setInput(testInput);
    if (!newManager.run()) {
        std::cerr << "Error running loaded FSM: " << newManager.getLastError() << std::endl;
    }
    std::string loadedOutput = newManager.getOutput();
    std::cout << "Loaded FSM output: " << loadedOutput << std::endl;
    
    // Compare outputs
    std::cout << "Comparing outputs: " << (originalOutput == loadedOutput ? "Identical!" : "Different!") << std::endl;
    
    // Trace execution for both FSMs
    std::cout << "\n=== Comparing execution traces ===\n" << std::endl;
    
    // Reset both FSMs
    manager.reset();
    newManager.reset();
    
    // Set the same input
    manager.setInput(testInput);
    newManager.setInput(testInput);
    
    // Step through each FSM and record state transitions
    std::vector<std::string> originalStates;
    std::vector<std::string> loadedStates;
    
    // Step through original FSM
    originalStates.push_back(manager.getCurrentState());
    while (manager.step()) {
        originalStates.push_back(manager.getCurrentState());
    }
    
    // Step through loaded FSM
    loadedStates.push_back(newManager.getCurrentState());
    while (newManager.step()) {
        loadedStates.push_back(newManager.getCurrentState());
    }
    
    // Compare execution paths
    bool tracesMatch = (originalStates.size() == loadedStates.size());
    if (tracesMatch) {
        for (size_t i = 0; i < originalStates.size(); i++) {
            if (originalStates[i] != loadedStates[i]) {
                tracesMatch = false;
                break;
            }
        }
    }
    
    std::cout << "Original execution path (" << originalStates.size() << " states): ";
    for (const auto& state : originalStates) {
        std::cout << state << " -> ";
    }
    std::cout << "END" << std::endl;
    
    std::cout << "Loaded execution path (" << loadedStates.size() << " states): ";
    for (const auto& state : loadedStates) {
        std::cout << state << " -> ";
    }
    std::cout << "END" << std::endl;
    
    std::cout << "Execution traces match: " << (tracesMatch ? "Yes!" : "No!") << std::endl;
    
    // Get info about a state
    std::cout << "\n=== Getting info about a state ===\n" << std::endl;
    nlohmann::json stateInfo = newManager.getStateInfo("State1");
    std::cout << "State1 info: " << stateInfo.dump(2) << std::endl;
    
    // Reset the FSM
    std::cout << "\n=== Resetting the loaded FSM ===\n" << std::endl;
    newManager.reset();
    
    // Step through execution
    std::cout << "\n=== Step through execution with the loaded FSM ===\n" << std::endl;
    newManager.setInput("01010e");
    
    std::cout << "Stepping through FSM execution:" << std::endl;
    int stepCount = 0;
    while (newManager.step()) {
        stepCount++;
        std::cout << "\n--- Step " << stepCount << " ---" << std::endl;
        std::cout << "Current state: " << newManager.getCurrentState() << std::endl;
        std::cout << "Output so far: " << newManager.getOutput() << std::endl;
    }
    
    std::cout << "\n=== Done! ===\n" << std::endl;
    
    // Try intentionally modifying a state to test cross-validation
    std::cout << "\n=== Testing cross-validation with modified state ===\n" << std::endl;
    
    // Make a copy of the original FSM
    std::cout << "Creating a modified FSM..." << std::endl;
    FSMManager modifiedManager;
    modifiedManager.importFSMFromJson(originalFsmJson);
    
    // Modify a state in the copy
    std::cout << "Modifying a state in the copy..." << std::endl;
    modifiedManager.updateState("State0", "State0", "", '1', false, true, std::chrono::milliseconds(100), false);
    
    // Compare the modified FSM with the original
    std::cout << "Comparing modified FSM with original..." << std::endl;
    bool modifiedFsmsEqual = compareFSMs(manager, modifiedManager);
    std::cout << "Modified FSM comparison result: " << (modifiedFsmsEqual ? "Equivalent (Unexpected!)" : "Different (Expected)") << std::endl;
    
    // Test functional behavior of the modified FSM
    std::cout << "\n=== Testing functional behavior of modified FSM ===\n" << std::endl;
    
    // Reset both FSMs
    manager.reset();
    modifiedManager.reset();
    
    // Set the same input
    const std::string testInput2 = "01010e";
    manager.setInput(testInput2);
    modifiedManager.setInput(testInput2);
    
    // Run both FSMs
    manager.run();
    modifiedManager.run();
    
    // Compare outputs
    std::string originalOutput2 = manager.getOutput();
    std::string modifiedOutput = modifiedManager.getOutput();
    
    std::cout << "Original FSM output: " << originalOutput2 << std::endl;
    std::cout << "Modified FSM output: " << modifiedOutput << std::endl;
    std::cout << "Outputs match: " << (originalOutput2 == modifiedOutput ? "Yes (Unexpected!)" : "No (Expected)") << std::endl;

    std::cout << "\n=== Summary of validation tests ===\n" << std::endl;
    std::cout << "1. Original vs Loaded FSM structure: " << (fsmsEqual ? "Equivalent ✓" : "Different ✗") << std::endl;
    std::cout << "2. Original vs Loaded FSM output: " << (originalOutput == loadedOutput ? "Identical ✓" : "Different ✗") << std::endl;
    std::cout << "3. Original vs Loaded execution trace: " << (tracesMatch ? "Identical ✓" : "Different ✗") << std::endl;
    std::cout << "4. Original vs Modified FSM structure: " << (!modifiedFsmsEqual ? "Different ✓" : "Equivalent ✗") << std::endl;
    std::cout << "5. Original vs Modified FSM output: " << (originalOutput2 != modifiedOutput ? "Different ✓" : "Identical ✗") << std::endl;
    
    return 0;
}
