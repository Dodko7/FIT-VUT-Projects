#include "state.hpp"
#include "inputDeps.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include "fsm.hpp"
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

// Is it possible to use a library for JSON serialization/deserialization?
// #include <nlohmann/json.hpp>

class FSM {
    private:
        std::unordered_map<std::string, std::shared_ptr<State>> states; // Using unordered_map for better memory management
        std::string name;
        std::string description;
        std::chrono::milliseconds stepDelay; // Using std::chrono for precise and efficient time management
        std::deque<char> input; // Using deque for efficient input processing
        std::string output; // Same as above, consider structured output
        std::shared_ptr<State> startState; // Consider std::shared_ptr<State> for ownership clarity
        std::shared_ptr<State> currentState; // Same as above
        std::unordered_map<std::string, std::shared_ptr<State>> finalStates; // Use unordered_map for faster lookups
        std::unordered_map<std::string, char> allowedInputs; // Map of allowed inputs for each state
        machineState currentMachineState; // Added machineState for consistent state tracking
        std::chrono::milliseconds fsmRunTime; // Added fsmRunTime for tracking FSM execution time

    public:
        FSM();

        void setName(const std::string& name);
        void setDescription(const std::string& description);
        void addState(const std::string& name, const std::string& description, bool isFinal);
        void removeState(const std::string& name);
        void setStartState(const std::string& name);
        void addTransition(const std::string& fromState, const std::string& toState, char input);
        void removeTransition(const std::string& fromState, const std::string& toState);
        void run(const std::string& inputSequence);
        void debug();

        machineState getCurrentMachineState() const;
        void setCurrentMachineState(machineState state);
        void saveToJson(const std::string& filename);
        void loadFromJson(const std::string& filename);

        std::shared_ptr<State> getCurrentState() const;
        const std::unordered_map<std::string, std::shared_ptr<State>>& getStates() const;
        std::shared_ptr<State> getStartState() const;
        const std::unordered_map<std::string, std::shared_ptr<State>>& getFinalStates() const;
        std::shared_ptr<State> getStatePtrByName(std::string& name);
        std::vector<std::string> getAllStateNames() const;

        void validateFSM();
};

FSM::FSM() : startState(nullptr), currentState(nullptr), currentMachineState(machineState::IDLE), stepDelay(0) {}
// TO BE IMPLEMENTED AND REVISED WITH HEADER FILE
// void FSM::addState(const std::string& name, const std::string& description, bool isFinal, ) {
//     if (states.find(name) != states.end()) {
//         throw std::invalid_argument("State with the given name already exists.");
//     }

//     auto state = std::make_shared<State>(name, description, isFinal);
//     states[name] = state;

//     if (isFinal) {
//         finalStates[name] = state;
//     }
// }

void FSM::setName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("FSM name cannot be empty");
    }
    if (name.length() > 20) {
        throw std::invalid_argument("FSM name too long");
    }

    this->name = name;
}
void FSM::setDescription(const std::string& description) {
    if (description.empty()) {
        throw std::invalid_argument("FSM description cannot be empty");
    }
    if (description.length() > 100) {
        throw std::invalid_argument("FSM description too long");
    }
    this->description = description;
}

void FSM::removeState(const std::string& name) {
}

void FSM::setStartState(const std::string& name) {
}

void FSM::addTransition(const std::string& fromState, const std::string& toState, char input) {
}

void FSM::removeTransition(const std::string& fromState, const std::string& toState) {
}

void FSM::run(const std::string& inputSequence) {
}

void FSM::debug() {
    // Debugging logic to be implemented
}

std::shared_ptr<State> FSM::getCurrentState() const {
    return currentState;
}

const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getStates() const {
    return states;
}

std::shared_ptr<State> FSM::getStartState() const {
    return startState;
}

const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getFinalStates() const {
    return finalStates;
}

machineState FSM::getCurrentMachineState() const {
    return currentMachineState;
}

void FSM::setCurrentMachineState(machineState state) {
    currentMachineState = state;
}

// Placeholder for saving FSM to JSON
void FSM::saveToJson(const std::string& filename) {
    // TODO: Serialize FSM properties, states, and transitions into JSON format
}

// Placeholder for loading FSM from JSON
void FSM::loadFromJson(const std::string& filename) {
    // TODO: Deserialize FSM properties, states, and transitions from JSON format
}

void FSM::validateFSM() {
    // 1. Check reachability
    if (!startState) {
        std::cerr << "Start state is not set" << std::endl;
    }
    std::unordered_set<std::string> visited;
    std::function<void(const std::shared_ptr<State>&)> dfs;
    dfs = [&](const std::shared_ptr<State>& state) {
        if (!state) return;
        const std::string& name = state->getName();
        if (visited.count(name)) return;
        visited.insert(name);
        for (const auto& next : state->getNextStates()) {
            dfs(next);
        }
    };
    dfs(startState);
    for (const auto& pair : states) {
        if (!visited.count(pair.first)) {
            std::cerr << "State '" << pair.first << "' is unreachable from the start state" << std::endl;
            // Handle the unreachable state and disable running.
        }
    }

    // 2. Check determinism (no duplicate input symbols for transitions from the same state)
    for (const auto& pair : states) {
        const auto& state = pair.second;
        std::unordered_set<char> seenInputs;
        for (const auto& dep : state->getDependencies()) {
            char input = dep->getExpectedInput();
            if (seenInputs.count(input)) {
                std::cerr << "State '" << state->getName() << "' has multiple transitions for input '" << input << "'" << std::endl;
                // Handle the determinism violation and disable running.
            }
            seenInputs.insert(input);
        }
    }
}

std::shared_ptr<State> FSM::getStatePtrByName(std::string& name) {
    auto it = states.find(name);
    if (it != states.end()) {
        return it->second;
    }
    return nullptr; // Return nullptr if state not found
}

std::vector<std::string> FSM::getAllStateNames() const {
    std::vector<std::string> stateNames;
    for (const auto& pair : states) {
        stateNames.push_back(pair.first);
    }
    return stateNames;
}