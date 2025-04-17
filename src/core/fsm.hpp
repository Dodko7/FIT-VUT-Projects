#ifndef FSM_HPP
#define FSM_HPP

#include "state.hpp"
#include <string>
#include <vector>
#include <memory> // For smart pointers
#include <unordered_set> // For unordered_set

// Define machineState as an enum class for better type safety and accessibility
enum class machineState {
    IDLE,
    RUNNING,
    STOPPED,
    ERROR
};

class FSM {
    private:
        std::vector<std::shared_ptr<State>> states; // Use shared_ptr for better memory management
        std::string name; // Name of the FSM
        std::string description; // Description of the FSM
        float stepDelay; // Delay between steps in seconds
        std::string input; // Current input to the FSM
        std::string output; // Current output of the FSM
        std::shared_ptr<State> startState; // Use shared_ptr for ownership clarity
        std::shared_ptr<State> currentState; // Use shared_ptr for ownership clarity
        std::vector<std::shared_ptr<State>> finalStates; // Use shared_ptr for consistency
        std::unordered_set<machineState> machineStates; // Use unordered_set for faster lookups and to store unique machine states

        // To traverse the unordered_set, use a range-based for loop or an iterator, e.g.:
        // for (const auto& state : machineStates) {
        //     // Process each state
        // }
        machineState currentMachineState; // Tracks the current machine state

    public:
        FSM(); // Constructor
        ~FSM() = default; // Smart pointers handle memory management

        // Add a new state to the FSM
        void addState(const std::string& name, bool isFinal = false);

        // Remove a state from the FSM by name
        void removeState(const std::string& name);

        // Set the start state of the FSM
        void setStartState(const std::string& name);
        void setStartState(std::shared_ptr<State> state); // Added missing declaration

        // Add a transition between two states
        void addTransition(const std::string& fromState, const std::string& toState, char input);

        // Remove a transition between two states
        void removeTransition(const std::string& fromState, const std::string& toState);

        // Edit a state's properties
        void editState(const std::string& name, const std::string& newName, const std::string& output, bool isFinal);

        // Edit a transition's input condition
        void editTransition(const std::string& fromState, const std::string& toState, char newInput);

        // Save the FSM configuration to a file
        void saveToFile(const std::string& filename);

        // Load an FSM configuration from a file
        void loadFromFile(const std::string& filename);

        // Run the FSM with a given input sequence
        void run(const std::string& inputSequence);

        // Debug the FSM step by step
        void debug();

        // Get the current state of the FSM (for GUI integration)
        std::shared_ptr<State> getCurrentState() const;

        // Get all states in the FSM (for GUI integration)
        const std::vector<std::shared_ptr<State>>& getStates() const;

        // Get the start state of the FSM (for GUI integration)
        std::shared_ptr<State> getStartState() const;

        // Get the final states of the FSM (for GUI integration)
        const std::vector<std::shared_ptr<State>>& getFinalStates() const;

        // Get the current machine state
        machineState getCurrentMachineState() const;

        // Set the current machine state
        void setCurrentMachineState(machineState state);

        /**
         * @brief Save the FSM configuration to a JSON file.
         *
         * This method serializes the FSM, including its states and transitions, into a JSON file.
         *
         * @param filename The name of the file to save the FSM configuration.
         */
        void saveToJson(const std::string& filename);

        /**
         * @brief Load an FSM configuration from a JSON file.
         *
         * This method deserializes the FSM configuration from a JSON file, reconstructing its states and transitions.
         *
         * @param filename The name of the file to load the FSM configuration from.
         */
        void loadFromJson(const std::string& filename);
};

#endif // FSM_HPP