#include "state.hpp"
#include "inputDeps.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
// Is it possible to use a library for JSON serialization/deserialization?
// #include <nlohmann/json.hpp>

class FSM {
    private:
        std::vector<std::shared_ptr<State>> states; // Consider using std::shared_ptr<State> for better memory management
        std::string name;
        std::string description;
        float stepDelay; // Ensure this is appropriate for timing; consider std::chrono for precise delays
        std::string input; // Could be replaced with a more structured input type if needed
        std::string output; // Same as above, consider structured output
        std::shared_ptr<State> startState; // Consider std::shared_ptr<State> for ownership clarity
        std::shared_ptr<State> currentState; // Same as above
        std::vector<std::shared_ptr<State>> finalStates; // std::unordered_map for faster lookups?
        std::string possibleInputs; // std::set?
        machineState currentMachineState; // Added machineState for consistent state tracking

    public:
        FSM();

        void addState(const std::string& name, bool isFinal);
        void removeState(const std::string& name);
        void setStartState(std::shared_ptr<State> state);
        void autoConnectStates();
        void loadFromFile(const std::string& filename);
        void saveToFile(const std::string& filename);
        void run(const std::string& inputSequence);
        void debug();

        machineState getCurrentMachineState() const;
        void setCurrentMachineState(machineState state);
        void saveToJson(const std::string& filename);
        void loadFromJson(const std::string& filename);

        std::shared_ptr<State> getCurrentState() const;
        const std::vector<std::shared_ptr<State>>& getStates() const;
        std::shared_ptr<State> getStartState() const;
        const std::vector<std::shared_ptr<State>>& getFinalStates() const;
};

#include "fsm.hpp"

FSM::FSM() : startState(nullptr), currentState(nullptr), currentMachineState(machineState::IDLE) {}

// Destructor removed as smart pointers handle memory management

void FSM::addState(const std::string& name, bool isFinal) {
    auto state = std::make_shared<State>(name, isFinal);
    states.push_back(state);
    if (isFinal) {
        finalStates.push_back(state);
    }
}

void FSM::removeState(const std::string& name) {
    states.erase(std::remove_if(states.begin(), states.end(),
        [&name](const std::shared_ptr<State>& state) { return state->getName() == name; }), states.end());
    finalStates.erase(std::remove_if(finalStates.begin(), finalStates.end(),
        [&name](const std::shared_ptr<State>& state) { return state->getName() == name; }), finalStates.end());
}

std::shared_ptr<State> FSM::getCurrentState() const {
    return currentState;
}

const std::vector<std::shared_ptr<State>>& FSM::getStates() const {
    return states;
}

std::shared_ptr<State> FSM::getStartState() const {
    return startState;
}

const std::vector<std::shared_ptr<State>>& FSM::getFinalStates() const {
    return finalStates;
}

machineState FSM::getCurrentMachineState() const {
    return currentMachineState;
}

void FSM::setCurrentMachineState(machineState state) {
    currentMachineState = state;
}

void FSM::run(const std::string& inputSequence) {
    if (currentMachineState == machineState::IDLE) {
        setCurrentMachineState(machineState::RUNNING);
        // Logic for processing the input sequence
        // ...existing code...
    }
}

void FSM::debug() {
    if (currentMachineState == machineState::RUNNING) {
        // Debugging logic while FSM is running
        // ...existing code...
    } else if (currentMachineState == machineState::STOPPED) {
        // Handle debugging when FSM is stopped
        // ...existing code...
    }
}

// Placeholder for saving FSM to JSON
void FSM::saveToJson(const std::string& filename) {
    // TODO: Serialize FSM properties, states, and transitions into JSON format
}

// Placeholder for loading FSM from JSON
void FSM::loadFromJson(const std::string& filename) {
    // TODO: Deserialize FSM properties, states, and transitions from JSON format
}