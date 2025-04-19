#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it
#include "fsm.hpp" // Include FSM to access machineState definitions
#include "fsmErrors.hpp" // Include FSM errors for exception handling
#include <algorithm> // For std::find
#include <stdexcept> // For std::invalid_argument
#include <iostream> // For std::cerr

#include <string>
#include <vector>
#include <memory>

class State {
private:
    std::string name;
    machineState transToMachineState; // Updated to use machineState directly
    std::vector<std::unique_ptr<inputDeps>> dependencies;
    std::string output;
    std::vector<std::shared_ptr<State>> nextStates;
    std::shared_ptr<State> previousState;
    bool isFinal;

public:
    // Constructor
    State(const std::string& name, bool isFinal = false);

    // Destructor removed as smart pointers handle memory management

    // Getters and setters

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("State name empty");
        }

        this->name = name;
    }

    bool getIsFinal() const {
        return isFinal;
    }

    void setIsFinal(bool isFinal) {

        this->isFinal = isFinal;
    }

    void addDependency(std::unique_ptr<inputDeps> dependency) {
        if (std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end()) {
            std::cerr << "Dependency already exists! Determinism violation" << std::endl;
            return;
        }
        dependencies.push_back(std::move(dependency));
    }

    void addNextState(std::shared_ptr<State> nextState) {
        if (nextState == nullptr) {
            throw std::invalid_argument("Next state cannot be null");
        }
        if (std::find(nextStates.begin(), nextStates.end(), nextState) != nextStates.end()) {
            std::cerr << "Next state already exists! Determinism violation" << std::endl;
            return;
        }
        nextStates.push_back(std::shared_ptr<State>(nextState));
    }

    void removeNextStateFO(std::shared_ptr<State> nextState) {
        if (nextState == nullptr) {
            throw std::invalid_argument("Next state cannot be null");
        }

        auto it = std::find(nextStates.begin(), nextStates.end(), nextState);

        if (it == nextStates.end()) {
            std::cerr << "State not found present." << std::endl;
            return;
        }
        nextStates.erase(it);
    }

    void removeNextStateOccurances(std::shared_ptr<State> nextState) {
        if (nextState == nullptr) {
            throw std::invalid_argument("Next state cannot be null");
        }


        if (std::find(nextStates.begin(), nextStates.end(), nextState) == nextStates.end()) {
            std::cerr << "State not found. Cannot remove." << std::endl;
            return;
        }

        // Remove moves all instances not matching nextState
        // to the end of the vector, then erase removes them
        // from the vector

        nextStates.erase(
            std::remove(nextStates.begin(), nextStates.end(), nextState),
            nextStates.end()
        );
    }

    const std::string& getOutput() const {
        return output;
    }

    void setOutput(const std::string& output) {
        if (output.empty()) {
            throw std::invalid_argument("Output string empty");
        }

        this->output = output;
    }

    const std::vector<std::shared_ptr<State>>& getNextStates() const {
        if (nextStates.empty()) {
            std::cerr << "No next states available." << std::endl;
            return nextStates;
        }

        return nextStates;
    }

    std::shared_ptr<State> getPreviousState() const {
        if (previousState == nullptr) {
            std::cerr << "No previous state available." << std::endl;
            return nullptr;
        }

        return previousState;
    }
};