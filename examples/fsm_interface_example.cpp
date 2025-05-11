#include "fsm_interface.hpp"
#include <iostream>
#include <QtCore/QCoreApplication>

/**
 * @file fsm_interface_example.cpp
 * @brief Example demonstrating how to use the FSMManager interface class
 * 
 * This example shows the basic usage of the FSMManager class to create, 
 * manipulate, and execute a simple Finite State Machine.
 */

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
    
    // Create a new manager and load the FSM
    std::cout << "\n=== Creating a new manager and loading the FSM ===\n" << std::endl;
    FSMManager newManager;
    if (!newManager.loadFSM("binary_counter.json")) {
        std::cerr << "Error loading FSM: " << newManager.getLastError() << std::endl;
        return 1;
    }
    std::cout << "FSM loaded successfully" << std::endl;
    
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
    
    return 0;
}
