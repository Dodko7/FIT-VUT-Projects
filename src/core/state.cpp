#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it
#include "fsm.hpp" // Include FSM to access machineState definitions

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
        dependencies.push_back(std::move(dependency));
    }

    void addNextState(std::shared_ptr<State> nextState) {
        nextStates.push_back(std::shared_ptr<State>(nextState));
    }

    void removeNextState(std::shared_ptr<State> nextState) {
        nextStates.erase(std::remove_if(nextStates.begin(), nextStates.end(),
            [nextState](const std::shared_ptr<State>& state) { return state.get() == nextState.get(); }), nextStates.end());
    }

    const std::string& getOutput() const {
        return output;
    }

    void setOutput(const std::string& output) {
        this->output = output;
    }

    const std::vector<std::shared_ptr<State>>& getNextStates() const {
        return nextStates;
    }

    std::shared_ptr<State> getPreviousState() const {
        return previousState;
    }
};