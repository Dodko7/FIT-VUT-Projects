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
    std::unique_ptr<State> thisState;
    std::chrono::milliseconds delay;
    bool isFinal;

public:
    // Constructor
    // Pointer to the current instance if State has to be passed upon creation
    State(const std::string& name, machineState transToMachineState, std::vector<std::unique_ptr<inputDeps>> dependencies, std::string& output, std::vector<std::shared_ptr<State>> nextStates,std::shared_ptr<State> previousState, std::unique_ptr<State> thisState, bool isFinal, std::chrono::milliseconds delay)
        : name(name), transToMachineState(transToMachineState), dependencies(std::move(dependencies)), output(output), nextStates(nextStates), previousState(previousState), thisState(std::move(thisState)), isFinal(isFinal), delay(delay) {
        if (name.empty()) {
            throw std::invalid_argument("State name empty");
        }
        if (name.length() > 20) {
            throw std::invalid_argument("State name too long");
        }
        if (output.empty()) {
            throw std::invalid_argument("Output string empty");
        }
    }

    // Destructor removed as smart pointers handle memory management
   ~State() = default;

    // Getters and setters

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("State name empty");
        }
        if (name.length() > 20) {
            throw std::invalid_argument("State name too long");
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

    std::unique_ptr<inputDeps> getDependency(char input, std::shared_ptr<State> fromState) {
        for (const auto& dependency : dependencies) {
            if (dependency->getExpectedInput() == input && dependency->getFromState() == fromState) {
                return std::make_unique<inputDeps>(*dependency);
            }
        }
        std::cerr << "Dependency not found." << std::endl;
        return nullptr;
    }

    void removeDependency(std::unique_ptr<inputDeps> dependency) {
        auto it = std::find(dependencies.begin(), dependencies.end(), dependency);
        if (it == dependencies.end()) {
            std::cerr << "Dependency not present for this state." << std::endl;
            return;
        }
        dependencies.erase(it);
    }

    std::vector<std::unique_ptr<inputDeps>>& getDependencies() {
        return dependencies;
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

        // Remove moves all instances matching nextState
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

    std::vector<std::shared_ptr<State>>& getNextStates() {
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

    void changePreviousState(std::shared_ptr<State> previousState) {
        if (previousState == nullptr) {
            throw std::invalid_argument("Previous state is null");
        }

        if (previousState == this->previousState) {
            return;
        }

        this->previousState = previousState;
    }

    machineState getTransitionTo() const {
        return transToMachineState;
    }
};