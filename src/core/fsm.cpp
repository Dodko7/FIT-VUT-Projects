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

// Is it possible to use a library for JSON serialization/deserialization?
#include <nlohmann/json.hpp>

FSM::FSM() : startState(nullptr), currentState(nullptr), stepDelay(0), currentMachineState(machineState::IDLE) {}

void FSM::addState(const std::string& name, const std::string& description, bool isFinal) {
    if (states.find(name) != states.end()) {
        throw InvalidStateException("State already exists: " + name);
    }
    if (name.empty() || name.length() > 20) {
        throw std::invalid_argument("Invalid state name");
    }

    // Create a new state with all required arguments
    auto state = std::make_shared<State>(
        name,                               // State name
        machineState::IDLE,                 // Default machine state
        std::vector<std::unique_ptr<inputDeps>>(), // Empty dependencies
        description,                        // Output (description)
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

// Add a transition between two states
// Add a transition between two states
void FSM::addTransition(std::string& fromState, std::string& toState, char input) {
    auto from = getStatePtrByName(fromState);
    auto to = getStatePtrByName(toState);
    if (!from || !to) {
        throw InvalidStateException("Invalid state name");
    }
    if (input == '\0') {
        throw InvalidInputException("Input cannot be null");
    }
    // Check for determinism
    for (const auto& dep : from->getDependencies()) {
        if (dep->getExpectedInput() == input) {
            throw DeterminismViolationException("Duplicate input for state: " + fromState);
        }
    }
    auto dep = std::make_unique<inputDeps>(input, from);
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
            if (dep->getExpectedInput() == input) {
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
    // Visualize the FSM using Graphviz
    std::ofstream dotFile("../../assets/fsm_debug.dot"); // Open a DOT file for Graphviz output
    if (!dotFile.is_open()) { // Check if the file was opened successfully
        std::cerr << "Failed to open file for Graphviz output" << std::endl;
        return;
    }

    dotFile << "digraph FSM {" << std::endl; // Start the DOT graph
    dotFile << "    rankdir=LR;" << std::endl; // Left-to-right layout
    dotFile << "    node [shape=circle];" << std::endl; // Default node shape

    // Add states to the DOT graph
    for (const auto& pair : states) {
        const auto& state = pair.second;
        if (state == startState) { // Highlight the start state
            dotFile << "    \"" << state->getName() << "\" [shape=doublecircle, color=green];" << std::endl;
        } else if (finalStates.count(state->getName())) { // Highlight final states
            dotFile << "    \"" << state->getName() << "\" [shape=doublecircle, color=red];" << std::endl;
        } else { // Regular states
            dotFile << "    \"" << state->getName() << "\";" << std::endl;
        }
    }

    // Add transitions to the DOT graph
    for (const auto& pair : states) {
        const auto& state = pair.second;
        for (const auto& dep : state->getDependencies()) {
            for (const auto& nextState : state->getNextStates()) {
                if (nextState) { // Add an edge for each transition
                    dotFile << "    \"" << state->getName() << "\" -> \"" 
                            << nextState->getName() << "\" [label=\"" 
                            << dep->getExpectedInput() << "\"];" << std::endl;
                }
            }
        }
    }

    dotFile << "}" << std::endl; // End the DOT graph
    dotFile.close(); // Close the DOT file
}

// Get the current state of the FSM
std::shared_ptr<State> FSM::getCurrentState() const {
    return currentState;
}

// Get all states in the FSM
const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getStates() const {
    return states;
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