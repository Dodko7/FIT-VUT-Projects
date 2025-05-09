#ifndef STATE_HPP
#define STATE_HPP

#include "fsm.hpp" // Ensure machineState is known
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "inputDeps.hpp"

enum class machineState;

class State {
private:
    std::string name;
    std::vector<std::shared_ptr<State>> nextStates;
    bool isFinal;
    std::vector<std::unique_ptr<inputDeps>> dependencies;
    std::chrono::milliseconds stepDelay; // The delay on-entry to this state
    std::string action; // JavaScript code for state action
    std::shared_ptr<State> previousState;
    machineState transToMachineState;

public:
    State(const std::string& name, machineState transToMachineState, 
          std::vector<std::unique_ptr<inputDeps>> dependencies, 
          const std::string& action, 
          std::vector<std::shared_ptr<State>> nextStates, 
          std::shared_ptr<State> previousState, bool isFinal);
    ~State() = default;

    const std::string& getName() const;
    void setName(const std::string& name);
    bool getIsFinal() const;
    void setIsFinal(bool isFinal);
    void addDependency(std::unique_ptr<inputDeps> dependency);
    std::unique_ptr<inputDeps> getDependency(char input, std::shared_ptr<State> fromState);
    std::unique_ptr<inputDeps> getDependency(const std::string& input, std::shared_ptr<State> fromState);
    void removeDependency(std::unique_ptr<inputDeps> dependency);
    std::vector<std::unique_ptr<inputDeps>>& getDependencies();
    const std::vector<std::unique_ptr<inputDeps>>& getDependencies() const;
    void addNextState(std::shared_ptr<State> nextState);
    void removeNextStateFO(std::shared_ptr<State> nextState);
    void removeNextStateOccurances(std::shared_ptr<State> nextState);
    const std::string& getAction() const; // New: Get JavaScript action
    void setAction(const std::string& action); // New: Set JavaScript action
    std::vector<std::shared_ptr<State>>& getNextStates();
    const std::vector<std::shared_ptr<State>>& getNextStates() const;
    std::shared_ptr<State> getPreviousState() const;
    void changePreviousState(std::shared_ptr<State> previousState);
    machineState getTransitionTo() const;
    int getStepDelay() const;
    
    // Additional methods if needed
    machineState getMachineState() const;
    void setMachineState(machineState state);
};

#endif // STATE_HPP