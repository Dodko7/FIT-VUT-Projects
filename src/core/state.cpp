#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it
#include "fsm.hpp" // Include FSM to access machineState definitions

#include <string>
#include <vector>
#include <memory>

class State {
private:
    std::string name;
    machineState currentMachineState; // Updated to use machineState directly
    std::vector<std::unique_ptr<inputDeps>> dependencies;
    std::string output;
    std::vector<std::shared_ptr<State>> nextStates;
    State* previousState;
    bool isFinal;

public:
    // Constructor
    State(const std::string& name, bool isFinal = false);

    // Destructor removed as smart pointers handle memory management

    // Getters and setters
    const std::string& getName() const;
    void setName(const std::string& name);

    bool getIsFinal() const;
    void setIsFinal(bool isFinal);

    void addDependency(std::unique_ptr<inputDeps> dependency);
    void addNextState(State* nextState);
    void removeNextState(State* nextState);

    machineState getMachineState() const;
    void setMachineState(machineState newState);

    const std::string& getOutput() const;
    void setOutput(const std::string& output);

    const std::vector<std::shared_ptr<State>>& getNextStates() const;

    void executeAction() const;

    // Other methods TBD
};

State::State(const std::string& name, bool isFinal)
    : name(name), isFinal(isFinal), currentMachineState(machineState::IDLE), previousState() {}

// Destructor removed as smart pointers handle memory management

machineState State::getMachineState() const {
    return currentMachineState;
}

void State::setMachineState(machineState newState) {
    currentMachineState = newState;
}

const std::string& State::getOutput() const {
    return output;
}

void State::setOutput(const std::string& output) {
    this->output = output;
}

const std::vector<std::shared_ptr<State>>& State::getNextStates() const {
    return nextStates;
}

void State::addNextState(State* nextState) {
    nextStates.push_back(std::shared_ptr<State>(nextState));
}

void State::removeNextState(State* nextState) {
    nextStates.erase(std::remove_if(nextStates.begin(), nextStates.end(),
        [nextState](const std::shared_ptr<State>& state) { return state.get() == nextState; }), nextStates.end());
}

void State::addDependency(std::unique_ptr<inputDeps> dependency) {
    dependencies.push_back(std::move(dependency));
}

void State::executeAction() const {
    if (currentMachineState == machineState::RUNNING) {
        // Execute action logic for RUNNING state
        // ...existing code...
    } else if (currentMachineState == machineState::STOPPED) {
        // Handle STOPPED state logic
        // ...existing code...
    }
}