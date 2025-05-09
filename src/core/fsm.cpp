#include "fsm.hpp" // Include FSM header
#include "state.hpp" // Include State class
#include "inputDeps.hpp" // Include InputDeps for managing transitions
#include "fsmErrors.hpp" // Include FSM-specific exceptions
#include <iostream> // For standard input/output
#include <string> // For string operations
#include <vector> // For vector container
#include <fstream> // For file operations
#include <chrono> // For time management
#include <stdexcept> // For exception handling
#include <unordered_set> // For unordered_set container
#include <unordered_map> // For unordered_map container
#include <algorithm> // For algorithms like std::find_if
#include <cstdlib> // Include standard library for system commands
#include "nlohmann/json.hpp" // Include JSON library for serialization/deserialization

using json = nlohmann::json;

FSM::FSM() : startState(nullptr), currentState(nullptr), stepDelay(0), currentMachineState(machineState::IDLE) {}

void FSM::addState(const std::string& name, const std::string& action, bool isFinal, std::chrono::milliseconds stepDelay) {
    if (states.find(name) != states.end()) {
        throw InvalidStateException("State already exists: " + name);
    }
    if (name.empty() || name.length() > 20) {
        throw std::invalid_argument("Invalid state name");
    }

    auto state = std::make_shared<State>(
        name,                               // State name
        machineState::IDLE,                 // Default machine state
        std::vector<std::unique_ptr<inputDeps>>(), // Empty dependencies
        action,                             // Action
        stepDelay,                          // Step delay
        std::vector<std::shared_ptr<State>>(), // Empty next states
        nullptr,                            // No previous state
        isFinal                             // Is final state
    );

    states[name] = state;
    if (isFinal) {
        finalStates[name] = state;
    }
}

void FSM::setName(const std::string& name) {
    if (name.empty()) { // Validate that the name is not empty
        throw std::invalid_argument("FSM name cannot be empty");
    }
    if (name.length() > 20) { // Validate that the name is not too long
        throw std::invalid_argument("FSM name too long");
    }
    this->name = name; // Set the FSM name
}

// Set the description of the FSM
void FSM::setDescription(const std::string& description) {
    if (description.empty()) { // Validate that the description is not empty
        throw std::invalid_argument("FSM description cannot be empty");
    }
    if (description.length() > 100) { // Validate that the description is not too long
        throw std::invalid_argument("FSM description too long");
    }
    this->description = description; // Set the FSM description
}

// Add these method implementations

const std::string& FSM::getName() const {
    return name;
}

const std::string& FSM::getDescription() const {
    return description;
}

// Public: Remove a state and recursively prune unreachable children
void FSM::removeState(const std::string& name) {
    deleteStateRecursive(name); // Call helper function to delete state recursively
}

// Set the start state of the FSM
void FSM::setStartState(const std::string& name) {
    auto it = states.find(name);
    if (it == states.end()) {
        throw InvalidStateException("State does not exist: " + name);
    }
    startState = it->second;
}

void FSM::addTransition(const std::string& fromState, const std::string& toState, const std::string& event, const std::string& condition, const std::string& timeout, const std::string& output) {
    auto from = getStatePtrByName(fromState);
    auto to = getStatePtrByName(toState);
    if (!from || !to) {
        throw InvalidStateException("Invalid state name");
    }
    // Check for determinism
    for (const auto& dep : from->getDependencies()) {
        if (dep->getEvent() == event && dep->getCondition() == condition && dep->getTimeout() == timeout) {
            throw DeterminismViolationException("Duplicate transition for state: " + fromState);
        }
    }
    auto dep = std::make_unique<inputDeps>(event, condition, timeout, output, from);
    from->addDependency(std::move(dep));
    from->addNextState(to);
}

// Remove a transition between two states
void FSM::removeTransition(std::string& fromState, std::string& toState, char input) {
    if (fromState.empty() || toState.empty()) {
        throw InvalidArgumentException("State names cannot be empty");
    }

    if (!findStateExists(fromState)) {
        throw InvalidStateException("State does not exist: " + fromState);
    }

    if (!findStateExists(toState)) {
        throw InvalidStateException("State does not exist: " + toState);
    }

    auto from = getStatePtrByName(const_cast<std::string&>(fromState));
    auto to = getStatePtrByName(const_cast<std::string&>(toState));

    if (!from || !to) {
        throw InvalidStateException("Invalid state pointers for transition removal");
    }

    // Remove the transition from the 'from' state
    from->removeNextStateOccurances(to);

    // Remove the dependency from the 'to' state
    to->removeDependency(to->getDependency(input, from));
}

bool FSM::findStateExists(const std::string& name) const {
    if (name.empty()) {
        throw InvalidArgumentException("State name cannot be empty");
    }
    return states.find(name) != states.end();
}

void FSM::addExpectedInput(const char value) {
    /**
     * @brief Adds an input to the FSM.
     * @param name The name of the input.
     * @param value The initial value of the input.
     * @throws std::invalid_argument If the name is empty, too long, or already exists.
     */
    if (value == '\0') {
        throw std::invalid_argument("Input cannot be null");
    }
    if(expectedInputs.find(value) != expectedInputs.end()) {
        throw std::invalid_argument("Input already exists: " + value);
    }
    expectedInputs.insert(value); // Add the input to the set
}


void FSM::removeExpectedInput(const char value) {
    expectedInputs.erase(value); // Ignore if input doesn't exist
}

bool FSM::checkValidInput() {
    /**
     * @brief Checks if the input is valid.
     * @return True if the input is valid, false otherwise.
     */

    char input = this->input[0]; // Get the first character of the input
    return expectedInputs.find(input) != expectedInputs.end();
}


void FSM::addVariable(const std::string& name, const std::string& value) {
    if (name.empty()) {
        throw std::invalid_argument("Variable name cannot be empty");
    }
    if (variables.find(name) != variables.end()) {
        throw std::invalid_argument("Variable already exists: " + name);
    }
    variables[name] = value;
}

void FSM::removeVariable(const std::string& name) {
    variables.erase(name);
}

const std::unordered_map<std::string, std::string>& FSM::getVariables() const {
    return variables;
}

// Run the FSM with a given input sequence
void FSM::run(const std::string& inputSequence) {
    if (!startState) {
        throw MooreMachineValidationException("No start state defined");
    }
    currentState = startState;
    currentMachineState = machineState::RUNNING;
    std::cout << "Starting FSM at state: " << currentState->getName() << "\n";

    for (char input : inputSequence) {
        bool transitioned = false;
        for (const auto& dep : currentState->getDependencies()) {
            // Convert the input char to string for comparison
            std::string inputStr(1, input);
            if (dep->getEvent() == inputStr) {  // Changed from getExpectedInput()
                for (const auto& next : currentState->getNextStates()) {
                    if (next && dep->getFromState() == currentState) {
                        currentState = next;
                        transitioned = true;
                        std::cout << "Transition on input '" << input << "' to state: " << currentState->getName() << "\n";
                        break;
                    }
                }
                break;
            }
        }
        if (!transitioned) {
            std::cout << "No transition for input '" << input << "' in state: " << currentState->getName() << "\n";
        }
    }
    currentMachineState = machineState::STOPPED;
    std::cout << "FSM stopped at state: " << currentState->getName() << "\n";
}

// TO BE DEBUGGED/TESTED
void FSM::debug() {
    std::ofstream dotFile("assets/fsm_debug.dot");
    if (!dotFile.is_open()) {
        std::cerr << "Error: Failed to open assets/fsm_debug.dot for writing" << std::endl;
        return;
    }

    // Start DOT graph
    dotFile << "digraph FSM {\n";
    dotFile << "    rankdir=LR;\n";
    dotFile << "    node [shape=circle];\n";

    // Add states
    for (const auto& pair : states) {
        const auto& state = pair.second;
        std::string attributes;
        if (state == startState) {
            attributes = "[shape=doublecircle, color=green]";
        } else if (finalStates.count(state->getName())) {
            attributes = "[shape=doublecircle, color=red]";
        } else {
            attributes = "";
        }
        dotFile << "    \"" << state->getName() << "\" " << attributes << ";\n";
    }

    // Add transitions
    for (const auto& pair : states) {
        const auto& state = pair.second;
        for (size_t i = 0; i < state->getDependencies().size(); ++i) {
            const auto& dep = state->getDependencies()[i];
            const auto& nextState = state->getNextStates()[i];
            if (nextState) {
                dotFile << "    \"" << state->getName() << "\" -> \"" 
                        << nextState->getName() << "\" [label=\"" 
                        << dep->getEvent() << "\"];\n";  // Changed from getExpectedInput()
            }
        }
    }

    dotFile << "}\n";
    dotFile.close();
}

// Get the current state of the FSM
std::shared_ptr<State> FSM::getCurrentState() const {
    return currentState;
}

// Get all states in the FSM
const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getStates() const {
    return states;
}

// Get the input string of FSM
std::string FSM::getInput() const {
    return input;
}

// Get the current output of the FSM
std::string FSM::getOutput() const {
    return output;
}

std::unordered_set<char> FSM::getExpectedInputs() const {
    return expectedInputs;
}

// Get the start state of the FSM
std::shared_ptr<State> FSM::getStartState() const {
    return startState;
}

// Get all final states in the FSM
const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getFinalStates() const {
    return finalStates;
}

// Get the current machine state
machineState FSM::getCurrentMachineState() const {
    return currentMachineState;
}

// Set the current machine state
void FSM::setCurrentMachineState(machineState state) {
    if (state != machineState::IDLE && state != machineState::RUNNING && state != machineState::STOPPED) {
        throw InvalidArgumentException("Invalid machine state");
    }
    currentMachineState = state;
}

void FSM::saveToJson(const std::string& filename) {
    json j;

    // Basic information - ensure name and description are saved
    j["name"] = name;
    j["description"] = description;
    j["startState"] = startState ? startState->getName() : "";
    j["currentState"] = currentState ? currentState->getName() : "";
    j["finalStates"] = json::array();
    for (const auto& pair : finalStates) {
        j["finalStates"].push_back(pair.second->getName());
    }

    // Inputs
    j["input"] = input;

    // Outputs
    j["output"] = output;

    // Expected inputs
    j["expectedInputs"] = json::array();
    for (const auto& input : expectedInputs) {
        j["expectedInputs"].push_back(std::string(1, input));
    }


    // Step delay
    j["stepDelay"] = stepDelay.count(); // Save step delay in milliseconds

    // Current machine state
    j["currentMachineState"] = static_cast<int>(currentMachineState);

    // Variables
    j["variables"] = variables;

    // States
    j["states"] = json::array();
    for (const auto& pair : states) {
        json state;
        state["name"] = pair.second->getName();
        state["action"] = pair.second->getAction();
        state["isFinal"] = pair.second->getIsFinal();
        state["stepDelay"] = pair.second->getStepDelay(); // Save step delay in milliseconds
        j["states"].push_back(state);
    }

    // Transitions
    j["transitions"] = json::array();
    for (const auto& pair : states) {
        const auto& state = pair.second;
        for (const auto& dep : state->getDependencies()) {
            for (const auto& nextState : state->getNextStates()) {
                if (nextState && dep->getFromState() == state) {
                    json transition;
                    transition["from"] = state->getName();
                    transition["to"] = nextState->getName();
                    transition["event"] = dep->getEvent();
                    transition["condition"] = dep->getCondition();
                    transition["timeout"] = dep->getTimeout();
                    transition["output"] = dep->getOutput();
                    j["transitions"].push_back(transition);
                }
            }
        }
    }

    // Save to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    file << j.dump(4); // Pretty print with indentation
    file.close();
}

void FSM::loadFromJson(const std::string& filename) {
    // Load file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        throw std::runtime_error("Invalid JSON format: " + std::string(e.what()));
    }
    file.close();

    // Clear current FSM
    states.clear();
    finalStates.clear();
    inputs.clear();
    outputs.clear();
    variables.clear();
    startState = nullptr;
    currentState = nullptr;
    currentMachineState = machineState::IDLE;

    // Load basic information - properly extract name and description
    if (j.contains("name")) {
        setName(j["name"].get<std::string>());
    }
    if (j.contains("description")) {
        setDescription(j["description"].get<std::string>());
    }

    // Load inputs
    if (j.contains("inputs")) {
        for (const auto& item : j["inputs"].items()) {
            addInput(item.key(), item.value().get<std::string>());
        }
    }

    // Load outputs
    if (j.contains("outputs")) {
        for (const auto& item : j["outputs"].items()) {
            addOutput(item.key(), item.value().get<std::string>());
        }
    }

    // Load variables
    if (j.contains("variables")) {
        for (const auto& item : j["variables"].items()) {
            addVariable(item.key(), item.value().get<std::string>());
        }
    }

    // Load states
    if (j.contains("states")) {
        for (const auto& state : j["states"]) {
            std::string name = state["name"].get<std::string>();
            std::string action = state.contains("action") ? 
                                state["action"].get<std::string>() : "";
            bool isFinal = state.contains("isFinal") && state["isFinal"].get<bool>();
            addState(name, action, isFinal); // Description/output removed
        }
    }

    // Load transitions
    if (j.contains("transitions")) {
        for (const auto& transition : j["transitions"]) {
            std::string from = transition["from"].get<std::string>();
            std::string to = transition["to"].get<std::string>();
            std::string event = transition.contains("event") ? 
                               transition["event"].get<std::string>() : "";
            std::string condition = transition.contains("condition") ? 
                                   transition["condition"].get<std::string>() : "";
            std::string timeout = transition.contains("timeout") ? 
                                 transition["timeout"].get<std::string>() : "";
            std::string output = transition.contains("output") ? 
                                 transition["output"].get<std::string>() : "";
            try {
                addTransition(from, to, event, condition, timeout, output);
            } catch (const DeterminismViolationException& e) {
                std::cerr << "Warning: " << e.what() << " (skipped during JSON loading)" << std::endl;
            }
        }
    }

    // Set start state
    if (j.contains("startState") && !j["startState"].get<std::string>().empty()) {
        setStartState(j["startState"].get<std::string>());
    }

    // Validate loaded FSM
    validateFSM();
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
        std::unordered_set<std::string> seenInputs;  // Changed from char to string
        for (const auto& dep : state->getDependencies()) {
            std::string input = dep->getEvent();  // Changed from getExpectedInput()
            if (seenInputs.count(input)) {
                std::cerr << "State '" << state->getName() << "' has multiple transitions for input '" << input << "'" << std::endl;
                // Handle the determinism violation and disable running.
            }
            seenInputs.insert(input);
        }
    }
}

std::shared_ptr<State> FSM::getStatePtrByName(const std::string& name) {
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

// Helper: Check if a state is referenced in any nextStates except from the current parent
bool FSM::isStateReferencedElsewhere(const std::string& stateName, const std::string& parentName) {
    for (const auto& pair : states) {
        if (pair.first == parentName) continue;
        const auto& nextStates = pair.second->getNextStates();
        for (const auto& next : nextStates) {
            if (next && next->getName() == stateName) {
                return true;
            }
        }
    }
    return false;
}

// Helper: Recursively prune unreachable states
void FSM::pruneUnreachableStates(const std::shared_ptr<State>& state, std::unordered_set<std::string>& visited, const std::string& parentName) {
    if (!state) return;
    const std::string& name = state->getName();
    if (visited.count(name)) return;
    visited.insert(name);

    // For each child, check if it is referenced elsewhere
    std::vector<std::shared_ptr<State>> children = state->getNextStates();
    for (const auto& child : children) {
        if (!child) continue;
        const std::string& childName = child->getName();
        if (!isStateReferencedElsewhere(childName, name)) {
            // Recursively prune this child
            pruneUnreachableStates(child, visited, name);
            // Remove from all relevant lists
            // Remove from parent's nextStates
            auto& parentNextStates = state->getNextStates();
            auto it = std::remove_if(parentNextStates.begin(), parentNextStates.end(), [&](const std::shared_ptr<State>& s) {
                return s && s->getName() == childName;
            });
            parentNextStates.erase(it, parentNextStates.end());
            // Remove from FSM's states map
            states.erase(childName);
            // Remove from finalStates if present
            finalStates.erase(childName);
            // Remove inputDeps referencing this state
            // (Assume inputDeps are only in the state being deleted)
            // Destructor will be called when shared_ptr refcount drops to zero
        }
    }
}

// Public method to prune unreachable states
void FSM::pruneUnreachable() {
    std::unordered_set<std::string> visited;
    pruneUnreachableStates(startState, visited, "");
}

// Helper: Remove all references to a state from other states' nextStates and dependencies
void FSM::removeReferencesToState(const std::string& stateName) {
    for (auto& pair : states) {
        auto& state = pair.second;
        // Remove from nextStates
        auto& nextStates = state->getNextStates();
        nextStates.erase(
            std::remove_if(nextStates.begin(), nextStates.end(), [&](const std::shared_ptr<State>& s) {
                return s && s->getName() == stateName;
            }),
            nextStates.end()
        );
        // Remove inputDeps referencing this state
        auto& deps = state->getDependencies();
        deps.erase(
            std::remove_if(deps.begin(), deps.end(), [&](const std::unique_ptr<inputDeps>& dep) {
                auto from = dep->getFromState();
                return from && from->getName() == stateName;
            }),
            deps.end()
        );
    }
}

// Helper: Recursively delete a state and its unreachable children
void FSM::deleteStateRecursive(const std::string& name) {
    auto it = states.find(name);
    if (it == states.end()) return;
    auto state = it->second;
    // For each child, check if it is referenced elsewhere
    std::vector<std::shared_ptr<State>> children = state->getNextStates();
    for (const auto& child : children) {
        if (!child) continue;
        const std::string& childName = child->getName();
        // Check if child is referenced from any other state (excluding this one)
        bool referenced = false;
        for (const auto& pair : states) {
            if (pair.first == name) continue;
            const auto& otherNext = pair.second->getNextStates();
            for (const auto& s : otherNext) {
                if (s && s->getName() == childName) {
                    referenced = true;
                    break;
                }
            }
            if (referenced) break;
        }
        if (!referenced) {
            deleteStateRecursive(childName);
        }
    }
    // Remove from all relevant lists
    removeReferencesToState(name);
    states.erase(name);
    finalStates.erase(name);
    // Smart pointers ensure destructors are called
}