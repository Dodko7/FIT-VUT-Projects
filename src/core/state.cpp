#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it
#include "fsm.hpp" // Include FSM to access machineState definitions
#include "fsmErrors.hpp" // Include FSM errors for exception handling
#include <algorithm> // For std::find
#include <stdexcept> // For std::invalid_argument
#include <iostream> // For std::cerr
#include <string>
#include <vector>
#include <memory>

/**
 * @class State
 * @brief Represents a state in the finite state machine (FSM).
 */
class State {
private:
    std::string name; // Name of the state
    machineState transToMachineState; // Machine state to transition to
    std::vector<std::unique_ptr<inputDeps>> dependencies; // Input dependencies for transitions
    std::string output; // Output associated with this state
    std::vector<std::shared_ptr<State>> nextStates; // States reachable from this state
    std::shared_ptr<State> previousState; // Previous state in the FSM
    std::unique_ptr<State> thisState; // Pointer to the current state instance
    std::chrono::milliseconds delay; // Delay before transitioning to the next state
    bool isFinal; // Indicates if this is a final state

public:
    // Constructor to initialize the state
    State(const std::string& name, machineState transToMachineState, std::vector<std::unique_ptr<inputDeps>> dependencies, 
          std::string& output, std::vector<std::shared_ptr<State>> nextStates, std::shared_ptr<State> previousState, 
          std::unique_ptr<State> thisState, bool isFinal, std::chrono::milliseconds delay)
        : name(name), transToMachineState(transToMachineState), dependencies(std::move(dependencies)), 
          output(output), nextStates(nextStates), previousState(previousState), thisState(std::move(thisState)), 
          isFinal(isFinal), delay(delay) {
        if (name.empty()) { // Validate that the state name is not empty
            throw std::invalid_argument("State name empty");
        }
        if (name.length() > 20) { // Validate that the state name is not too long
            throw std::invalid_argument("State name too long");
        }
        if (output.empty()) { // Validate that the output string is not empty
            throw std::invalid_argument("Output string empty");
        }
    }

    ~State() = default; // Default destructor as smart pointers handle memory

    // Get the name of the state
    const std::string& getName() const {
        return name;
    }

    // Set the name of the state
    void setName(const std::string& name) {
        if (name.empty()) { // Validate that the name is not empty
            throw std::invalid_argument("State name empty");
        }
        if (name.length() > 20) { // Validate that the name is not too long
            throw std::invalid_argument("State name too long");
        }
        this->name = name;
    }

    // Check if the state is a final state
    bool getIsFinal() const {
        return isFinal;
    }

    // Set whether the state is a final state
    void setIsFinal(bool isFinal) {
        this->isFinal = isFinal;
    }

    // Add a new input dependency to the state
    void addDependency(std::unique_ptr<inputDeps> dependency) {
        if (std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end()) {
            std::cerr << "Dependency already exists! Determinism violation" << std::endl;
            return;
        }
        dependencies.push_back(std::move(dependency));
    }

    // Get a specific input dependency based on input and originating state
    std::unique_ptr<inputDeps> getDependency(char input, std::shared_ptr<State> fromState) {
        for (const auto& dependency : dependencies) {
            if (dependency->getExpectedInput() == input && dependency->getFromState() == fromState) {
                return std::make_unique<inputDeps>(*dependency);
            }
        }
        std::cerr << "Dependency not found." << std::endl;
        return nullptr;
    }

    // Remove a specific input dependency
    void removeDependency(std::unique_ptr<inputDeps> dependency) {
        auto it = std::find(dependencies.begin(), dependencies.end(), dependency);
        if (it == dependencies.end()) {
            std::cerr << "Dependency not present for this state." << std::endl;
            return;
        }
        dependencies.erase(it);
    }

    // Get all input dependencies of the state
    std::vector<std::unique_ptr<inputDeps>>& getDependencies() {
        return dependencies;
    }

    // Add a new next state to the state
    void addNextState(std::shared_ptr<State> nextState) {
        if (nextState == nullptr) { // Validate that the next state is not null
            throw std::invalid_argument("Next state cannot be null");
        }
        if (std::find(nextStates.begin(), nextStates.end(), nextState) != nextStates.end()) {
            std::cerr << "Next state already exists! Determinism violation" << std::endl;
            return;
        }
        nextStates.push_back(std::shared_ptr<State>(nextState));
    }

    // Remove the first occurrence of a next state
    void removeNextStateFO(std::shared_ptr<State> nextState) {
        if (nextState == nullptr) { // Validate that the next state is not null
            throw std::invalid_argument("Next state cannot be null");
        }
        auto it = std::find(nextStates.begin(), nextStates.end(), nextState);
        if (it == nextStates.end()) {
            std::cerr << "State not found present." << std::endl;
            return;
        }
        nextStates.erase(it);
    }

    // Remove all occurrences of a next state
    void removeNextStateOccurances(std::shared_ptr<State> nextState) {
        if (nextState == nullptr) { // Validate that the next state is not null
            throw std::invalid_argument("Next state cannot be null");
        }
        if (std::find(nextStates.begin(), nextStates.end(), nextState) == nextStates.end()) {
            std::cerr << "State not found. Cannot remove." << std::endl;
            return;
        }

        // Remove moves all instances matching nextState
        // to the end of the vector, then erase removes them
        // from the vector

        nextStates.erase(
            std::remove(nextStates.begin(), nextStates.end(), nextState),
            nextStates.end()
        );
    }

    // Get the output associated with the state
    const std::string& getOutput() const {
        return output;
    }

    // Set the output associated with the state
    void setOutput(const std::string& output) {
        if (output.empty()) { // Validate that the output is not empty
            throw std::invalid_argument("Output string empty");
        }
        this->output = output;
    }

    // Get all next states reachable from this state
    std::vector<std::shared_ptr<State>>& getNextStates() {
        if (nextStates.empty()) {
            std::cerr << "No next states available." << std::endl;
        }
        return nextStates;
    }

    // Get the previous state of this state
    std::shared_ptr<State> getPreviousState() const {
        if (previousState == nullptr) {
            std::cerr << "No previous state available." << std::endl;
            return nullptr;
        }
        return previousState;
    }

    // Change the previous state of this state
    void changePreviousState(std::shared_ptr<State> previousState) {
        if (previousState == nullptr) { // Validate that the previous state is not null
            throw std::invalid_argument("Previous state is null");
        }
        if (previousState == this->previousState) {
            return;
        }
        this->previousState = previousState;
    }

    // Get the machine state to transition to
    machineState getTransitionTo() const {
        return transToMachineState;
    }
};