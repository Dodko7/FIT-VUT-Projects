#include "fsm.hpp"
#include "fsmErrors.hpp"
#include <iostream>

/**
 * @brief Main function to test FSM functionality.
 */
int main() {
    try {
        FSM fsm;
        // Set FSM properties
        fsm.setName("TestFSM");
        fsm.setDescription("Simple test FSM");

        // Add states
        fsm.addState("S0", "Initial state", false);
        fsm.addState("S1", "Intermediate state", false);
        fsm.addState("S2", "Final state", true);

        // Set start state
        fsm.setStartState("S0");

        // Add transitions
        std::string s0 = "S0", s1 = "S1", s2 = "S2";
        fsm.addTransition(s0, s1, 'a');
        fsm.addTransition(s1, s2, 'b');
        fsm.addTransition(s2, s0, 'c');

        // Validate FSM
        fsm.validateFSM();

        // Debug FSM (generate Graphviz visualization)
        fsm.debug();

        // Run FSM with a test input sequence
        std::cout << "Running FSM with input sequence: abc\n";
        fsm.run("abc");

    } catch (const FSMException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}