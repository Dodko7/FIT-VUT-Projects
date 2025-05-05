#ifndef FSM_HPP
#define FSM_HPP

#include "state.hpp"
#include <string>
#include <unordered_map>
#include <deque>
#include <chrono>

enum class machineState {
    IDLE,
    RUNNING,
    STOPPED,
    ERROR
};

class FSM {
private:
    std::unordered_map<std::string, std::shared_ptr<State>> states; // Efficient lookups by state name
    std::unordered_map<std::string, std::shared_ptr<State>> finalStates; // Efficient lookups for final states
    std::string name;
    std::string description;
    std::chrono::milliseconds stepDelay; // Precise timing
    std::deque<char> input; // Structured input type
    std::deque<char> output; // Structured output type
    std::shared_ptr<State> startState;
    std::shared_ptr<State> currentState;
    machineState currentMachineState;
    std::chrono::milliseconds fsmRunTime; // Track execution time

public:
    FSM();

    void addState(const std::string& name, bool isFinal);
    void removeState(const std::string& name);
    void setStartState(const std::string& name);
    void addTransition(const std::string& fromState, const std::string& toState, char input);
    void removeTransition(const std::string& fromState, const std::string& toState);
    void run(const std::string& inputSequence);
    void debug();

    std::shared_ptr<State> getCurrentState() const;
    const std::unordered_map<std::string, std::shared_ptr<State>>& getStates() const;
    std::shared_ptr<State> getStartState() const;
    const std::unordered_map<std::string, std::shared_ptr<State>>& getFinalStates() const;

    void validateFSM(); // Validate determinism and reachability
    void removeReferencesToState(const std::string& stateName);
    void deleteStateRecursive(const std::string& name);
};

#endif // FSM_HPP