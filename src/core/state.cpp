#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it
#include "fsm.hpp" // Include FSM to access machineState definitions
#include "fsmErrors.hpp" // Include FSM errors for exception handling
#include <algorithm> // For std::find
#include <stdexcept> // For std::invalid_argument
#include <iostream> // For std::cerr
#include <string>
#include <vector>
#include <memory>

// No class definition here, only method implementations for State

// Constructor with new action parameter
State::State(const std::string& name, machineState transToMachineState, std::vector<std::unique_ptr<inputDeps>> dependencies, const std::string& action, char output, std::chrono::milliseconds stepDelay, std::vector<std::shared_ptr<State>> nextStates, bool isFinal)
    : name(name), transToMachineState(transToMachineState), dependencies(std::move(dependencies)), action(action), output(output), stepDelay(stepDelay), nextStates(std::move(nextStates)), isFinal(isFinal) {
    if (name.empty() || name.length() > 20) {
        throw std::invalid_argument("Invalid state name");
    }
}

// Getters and setters

const std::string& State::getName() const {
    return name;
}

void State::setName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("State name empty");
    }
    if (name.length() > 20) {
        throw std::invalid_argument("State name too long");
    }

    this->name = name;
}

bool State::getIsFinal() const {
    return isFinal;
}

void State::setIsFinal(bool isFinal) {
    this->isFinal = isFinal;
}

void State::addDependency(std::unique_ptr<inputDeps> dependency) {
    if (std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end()) {
        std::cerr << "Dependency already exists! Determinism violation" << std::endl;
        return;
    }
    dependencies.push_back(std::move(dependency));
}

std::unique_ptr<inputDeps> State::getDependency(char input, std::shared_ptr<State> fromState) {
    for (const auto& dependency : dependencies) {
        if (dependency->getInput() == input && dependency->getFromState() == fromState) {
            return std::make_unique<inputDeps>(*dependency);
        }
    }
    std::cerr << "Dependency not found." << std::endl;
    return nullptr;
}

std::chrono::milliseconds State::getStepDelay() const {
    return stepDelay;
}

void State::removeDependency(std::unique_ptr<inputDeps> dependency) {
    auto it = std::find(dependencies.begin(), dependencies.end(), dependency);
    if (it == dependencies.end()) {
        std::cerr << "Dependency not present for this state." << std::endl;
        return;
    }
    dependencies.erase(it);
}

std::vector<std::unique_ptr<inputDeps>>& State::getDependencies() {
    return dependencies;
}

void State::addNextState(std::shared_ptr<State> nextState) {
    if (nextState == nullptr) {
        throw std::invalid_argument("Next state cannot be null");
    }
    if (std::find(nextStates.begin(), nextStates.end(), nextState) != nextStates.end()) {
        std::cerr << "Next state already exists! Determinism violation" << std::endl;
        return;
    }
    nextStates.push_back(std::shared_ptr<State>(nextState));
}

void State::removeNextStateFO(std::shared_ptr<State> nextState) {
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

void State::removeNextStateOccurances(std::shared_ptr<State> nextState) {
    if (nextState == nullptr) {
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

std::vector<std::shared_ptr<State>>& State::getNextStates() {
    return nextStates;
}

machineState State::getTransitionTo() const {
    return transToMachineState;
}

// New methods for JavaScript action support
const std::string& State::getAction() const {
    return action;
}

void State::setAction(const std::string& action) {
    this->action = action;
}

// Methods for Moore machine output support
char State::getOutput() const {
    return output;
}

void State::setOutput(char output) {
    this->output = output;
}