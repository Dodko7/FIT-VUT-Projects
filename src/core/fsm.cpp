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
#include <thread> // For sleep functionality
#include "nlohmann/json.hpp" // Include JSON library for serialization/deserialization
#include <set>  
#include <map>

using json = nlohmann::json;

FSM::FSM() : startState(nullptr), currentState(nullptr), stepDelay(0), currentMachineState(machineState::IDLE) {}

void FSM::addState(const std::string& name, const std::string& action, char output, bool isFinal, std::chrono::milliseconds stepDelay) {
    if (states.find(name) != states.end()) {
        throw InvalidStateException("State already exists: " + name);
    }
    if (name.empty() || name.length() > 20) {
        throw InvalidArgumentException("Invalid state name");
    }

    auto state = std::make_shared<State>(
        name,                               // State name
        std::nullopt,                 // Default machine state
        std::vector<std::unique_ptr<inputDeps>>(), // Empty dependencies
        action,                             // Action
        output,                             // Output (Moore machine) - single character
        stepDelay,                          // Step delay
        std::vector<std::shared_ptr<State>>(), // Empty next states
        isFinal                             // Is final state

    );

    states[name] = state;
    if (isFinal) {
        finalStates[name] = state;
    }
}

void FSM::setName(const std::string& name) {
    if (name.empty()) { // Validate that the name is not empty
        throw InvalidArgumentException("FSM name cannot be empty");
    }
    if (name.length() > 20) { // Validate that the name is not too long
        throw InvalidArgumentException("FSM name too long");
    }
    this->name = name; // Set the FSM name
}

// Set the description of the FSM
void FSM::setDescription(const std::string& description) {
    if (description.empty()) { // Validate that the description is not empty
        throw InvalidArgumentException("FSM description cannot be empty");
    }
    if (description.length() > 100) { // Validate that the description is not too long
        throw InvalidArgumentException("FSM description too long");
    }
    this->description = description; // Set the FSM description
}

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

void FSM::addTransition(const std::string& fromState, const std::string& toState, const std::string& event, const std::string& condition, const char input) {
    auto from = getStatePtrByName(fromState);
    auto to = getStatePtrByName(toState);
    if (!from || !to) {
        throw InvalidStateException("Invalid state name");
    }
    // Check for determinism
    for (const auto& dep : from->getDependencies()) {
        if (dep->getEvent() == event && dep->getCondition() == condition && dep->getInput() == input) {
            throw DeterminismViolationException("Duplicate transition for state: " + fromState);
        }
    }
    auto dep = std::make_unique<inputDeps>(event, condition, from, input);
    from->addDependency(std::move(dep));
    // Add next state only once
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

    auto from = getStatePtrByName(fromState);
    auto to = getStatePtrByName(toState);

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
     * @param value The name of the input.
     * @throws InvalidArgumentException If the value is empty or already exists.
     */
    if (value == '\0') {
        throw InvalidArgumentException("Input cannot be null");
    }
    if (expectedInputs.find(value) != expectedInputs.end()) {
        throw InvalidArgumentException("Input already exists: " + std::string(1, value));
    }
    expectedInputs.insert(value);
}

void FSM::removeExpectedInput(const char value) {
    expectedInputs.erase(value); // Ignore if input doesn't exist
}

bool FSM::checkValidInput() {

    char input = this->input[0]; // Get the first character of the input
    return expectedInputs.find(input) != expectedInputs.end();
}

void FSM::addOutput(const char value) {
    if (value == '\0') {
        throw InvalidArgumentException("Output cannot be null");
    }
    output += std::string(1, value); // Append the output character to the output string
}

void FSM::clearOutput() {
    output.clear(); // Clear the output string
}


void FSM::addVariable(const std::string& name, const std::string& value) {
    if (name.empty()) {
        throw InvalidArgumentException("Variable name cannot be empty");
    }
    if (variables.find(name) != variables.end()) {
        throw InvalidArgumentException("Variable already exists: " + name);
    }
    variables[name] = value;
}

void FSM::removeVariable(const std::string& name) {
    variables.erase(name);
}

const std::unordered_map<std::string, std::string>& FSM::getVariables() const {
    return variables;
}

// Make the current transition to the next state
void FSM::transitionToState() {
    if (!currentState) {
        throw InvalidStateException("Current state is null");
    }
    if (input.empty()) {
        this->setCurrentMachineState(machineState::STOPPED); // Stop the FSM if input is empty
        throw InvalidArgumentException("Input string is empty");
    }
    if (currentState) {
        if (getExpectedInputs().find(input[0]) == getExpectedInputs().end()) {
            throw InvalidArgumentException("Invalid input: " + std::string(1, input[0]));
        
            char& inputCharToProcess = this->input[0]; // Convert the first character of the input to a string
            for (auto& next : currentState->getNextStates()) {
                for (auto& dep : next->getDependencies()) {
                    if (dep->getInput() == inputCharToProcess && currentState == dep->getFromState()) { // Check if the input matches
                        std::cout << "Transitioning from state " << currentState->getName() << " to state " << next->getName() << " by input " << inputCharToProcess << "\n";
                        setCurrentState(next); // Transition to the next state
                        addOutput(currentState->getOutput()); // Add the output of the new state
                        discardInputChar(); // Discard the processed input character
    
                        if(currentState->getTransitionTo() != std::nullopt) { // Check if the state has a transition to another machine state
                            setCurrentMachineState(currentState->getTransitionTo().value()); // Set the machine state if defined
                        }
                        return;
                    }
                }
            }
        }
        throw InvalidArgumentException("Input cannot be processed because it is not defined in the input alphabet: " + std::string(1, input[0]) + "\n");
    }
}


// Run the FSM 
void FSM::run() {
    // Check initial conditions
    if (!startState) {
        throw MooreMachineValidationException("No start state defined");
    }
    
    if (input.empty()) {
        throw InvalidArgumentException("No input sequence provided");
    }
    
    // Validate the FSM before running
    try
    {
        validateFSM(); // Validate the FSM
    }
    catch(const std::exception& e)
    {
        // Handle validation errors
        std::cerr << "Running the FSM simulation failed: " << e.what() << '\n';
        return;
    }
    

    // Clear output
    clearOutput();
    
    // Initialize state
    setCurrentState(startState);
    setCurrentMachineState(machineState::RUNNING);
    
    // In a Moore machine, output the initial state's output character
    addOutput(currentState->getOutput());
    
    std::cout << "Starting FSM at state: " << currentState->getName() << "\n";
    
    // Main execution loop - continue until we're out of input or stopped
    while (currentMachineState == machineState::RUNNING && !input.empty()) {
        try {
            // Use the existing transitionToState function to process transitions
            transitionToState();

            // Check if the current state has its own delay
            std::chrono::milliseconds stateDelay = currentState->getStepDelay();
            if (stateDelay.count() > 0) {
                std::this_thread::sleep_for(stateDelay);
            }
            
            // Apply step delay if set
            if (stepDelay.count() > 0) {
                std::this_thread::sleep_for(stepDelay);
            }
            
            // Check if we've reached a final state
            if (currentState->getIsFinal()) {
                std::cout << "Reached final state: " << currentState->getName() << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during FSM execution: " << e.what() << "\n";
            setCurrentMachineState(machineState::ERROR);
            break;
        }
    }
    
    // Update machine state when done
    if (currentMachineState == machineState::RUNNING) {
        setCurrentMachineState(machineState::STOPPED);
    }
    
    std::cout << "FSM stopped at state: " << currentState->getName() << "\n";
    std::cout << "Final output: " << output << "\n";
}

// Debug step function - process next transition or stop if no more input
bool FSM::debugStep() {
    if (input.empty()) {
        setCurrentMachineState(machineState::STOPPED);
        std::cout << "Debug: No more input to process.\n";
        return false;
    }
    
    try {
        // Set machine state to running if not already
        if (currentMachineState != machineState::RUNNING) {
            setCurrentMachineState(machineState::RUNNING);
        }
        
        // Use existing transitionToState to process one transition
        transitionToState();
        
        // Check if we've reached a final state
        if (currentState->getIsFinal()) {
            std::cout << "Debug: Reached final state: " << currentState->getName() << "\n";
        }
        
        // Check if we're out of input
        if (input.empty()) {
            setCurrentMachineState(machineState::STOPPED);
            std::cout << "Debug: No more input to process.\n";
        }
        
        return true;
    } catch (const InvalidArgumentException& e) {
        // This is expected when input is empty
        setCurrentMachineState(machineState::STOPPED);
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error during debug step: " << e.what() << "\n";
        setCurrentMachineState(machineState::ERROR);
        return false;
    }
}

void FSM::debug() {
    /**
     * @brief Generates a Graphviz DOT file representing the FSM.
     * Uses the internal state of the FSM to ensure consistency with the automaton's specification.
     */
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
        }
        dotFile << "    \"" << state->getName() << "\" " << attributes << ";\n";
    }

    // Add transitions from internal FSM state with deduplication
    std::map<std::string, std::string> uniqueTransitions; // Key: from|event|condition|timeout, Value: to|label
    for (const auto& pair : states) {
        const auto& state = pair.second;
        const auto& deps = state->getDependencies();
        const auto& nextStates = state->getNextStates();

        // Iterate over dependencies and next states
        for (size_t i = 0; i < deps.size() && i < nextStates.size(); ++i) {
            const auto& dep = deps[i];
            const auto& nextState = nextStates[i];
            if (!nextState || dep->getFromState() != state) {
                continue;
            }

            // Create transition label
            std::string label;
            if (!dep->getTimeout().empty()) {
                label = "[" + dep->getTimeout() + "]";
            } else if (!dep->getEvent().empty()) {
                label = dep->getEvent();
                if (!dep->getCondition().empty()) {
                    label += " [" + dep->getCondition() + "]";
                }
            } else {
                continue; // Skip invalid transitions
            }

            // Create unique key for deduplication
            std::string key = state->getName() + "|" + dep->getEvent() + "|" + 
                             dep->getCondition() + "|" + dep->getTimeout();
            if (uniqueTransitions.find(key) != uniqueTransitions.end()) {
                std::cerr << "Warning: Duplicate transition detected for state: " 
                          << state->getName() << ", event: " << dep->getEvent() 
                          << ", condition: " << dep->getCondition() 
                          << ", timeout: " << dep->getTimeout() << std::endl;
                continue; // Skip duplicate
            }
            uniqueTransitions[key] = nextState->getName() + "|" + label;
        }
    }

    // Write unique transitions to DOT file
    for (const auto& [key, value] : uniqueTransitions) {
        // Extract toState and label from value
        auto pos = value.find('|');
        if (pos == std::string::npos) continue;
        std::string to = value.substr(0, pos);
        std::string label = value.substr(pos + 1);
        // Extract from state from key
        pos = key.find('|');
        std::string from = key.substr(0, pos);
        dotFile << "    \"" << from << "\" -> \"" << to 
                << "\" [label=\"" << label << "\"];\n";
    }

    dotFile << "}\n";
    dotFile.close();
}

// Get the current state of the FSM
std::shared_ptr<State> FSM::getCurrentState() const {
    return currentState;
}

void FSM::setCurrentState(std::shared_ptr<State> state) {
    if (!state) {
        throw InvalidArgumentException("State cannot be null");
    }

    if (states.find(state->getName()) == states.end()) {
        throw InvalidStateException("State does not exist: " + state->getName());
    }

    currentState = state; // Set the current state
}



// Get all states in the FSM
const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getStates() const {
    return states;
}

void FSM::setInput(const std::string& input) {
    if (input.empty()) {
        throw InvalidArgumentException("Input cannot be empty");
    }
    if (input.length() > 100) {
        throw InvalidArgumentException("Input too long");
    }
    // Validate that the input contains only expected characters
    for (char c : input) {
        if (expectedInputs.find(c) == expectedInputs.end()) {
            throw InvalidArgumentException("Input contains unexpected character: " + std::string(1, c));
        }
    }

    this->input = input; // Set the FSM input
}

// Get the input string of FSM
std::string FSM::getInput() const {
    return input;
}

// Discard the first character of the input string
void FSM::discardInputChar() {
    if (!input.empty()) {
        input.erase(0, 1); // Remove the first character from the input string
    }
}

// Get the current output of the FSM
std::string FSM::getOutput() const {
    return output;
}

void FSM::clearOutput() {
    output.clear(); // Clear the output string
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
    if (state < machineState::IDLE || state > machineState::ERROR) {
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
        j["expectedInputs"].push_back(input);
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
        state["output"] = std::string(1, pair.second->getOutput()); // Save output character as string
        state["isFinal"] = pair.second->getIsFinal();
        state["stepDelay"] = pair.second->getStepDelay().count(); // Save step delay in milliseconds
        j["states"].push_back(state);
    }

    // Transitions
    // Transitions with deduplication
    j["transitions"] = json::array();
    std::set<std::string> seenTransitions; // Key: from|to|event|condition|timeout
    for (const auto& pair : states) {
        const auto& state = pair.second;
        const auto& deps = state->getDependencies();
        const auto& nextStates = state->getNextStates();
        for (size_t i = 0; i < deps.size() && i < nextStates.size(); ++i) {
            const auto& dep = deps[i];
            const auto& nextState = nextStates[i];
            if (!nextState || dep->getFromState() != state) {
                continue;
            }
            // Create unique key for transition
            std::string key = state->getName() + "|" + nextState->getName() + "|" +
                             dep->getEvent() + "|" + dep->getCondition() + "|" + 
                             dep->getTimeout();
            if (seenTransitions.find(key) != seenTransitions.end()) {
                continue; // Skip duplicate
            }
            seenTransitions.insert(key);
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
    input.clear();
    output.clear();
    variables.clear();
    startState = nullptr;
    currentState = nullptr;
    expectedInputs.clear();
    setCurrentMachineState(machineState::IDLE);
    stepDelay = std::chrono::milliseconds(0);

    // Load basic information - properly extract name and description
    if (j.contains("name")) {
        setName(j["name"].get<std::string>());
    }
    if (j.contains("description")) {
        setDescription(j["description"].get<std::string>());
    }

    // Load input
    if (j.contains("input")) {
        setInput(j["input"].get<std::string>());
    }

    // // Load output
    // if (j.contains("output")) {
    //     for (const auto& item : j["outputs"].items()) {
    //         addOutput(item.key(), item.value().get<std::string>());
    //     }
    // }

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
            
            // Extract output character from string (use first character or default to '\0')
            char output = '\0';
            if (state.contains("output")) {
                std::string outputStr = state["output"].get<std::string>();
                if (!outputStr.empty()) {
                    output = outputStr[0]; // Take only the first character
                }
            }
            
            bool isFinal = state.contains("isFinal") && state["isFinal"].get<bool>();
            std::chrono::milliseconds stateDelay = state.contains("stepDelay") ? 
                                                 std::chrono::milliseconds(state["stepDelay"].get<int>()) : 
                                                 std::chrono::milliseconds(0);
            addState(name, action, output, isFinal, stateDelay);
        }
    }

    // Load transitions
    if (j.contains("transitions")) {
        for (const auto& transition : j["transitions"]) {
            try {
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
                
                // Try to add the transition, but catch and just warn about determinism violations
                try {
                    addTransition(from, to, event, condition, timeout, output);
                } catch (const DeterminismViolationException& e) {
                    std::cerr << "Warning: " << e.what() << " (skipped during JSON loading)" << std::endl;
                    // Skip this transition and continue with others
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("Error loading transition: " + std::string(e.what()));
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
    std::vector<std::string> validationErrors;

    // 1. Check that a start state is defined
    if (!startState) {
        validationErrors.push_back("No start state defined");
        throw MooreMachineValidationException("No start state defined");
    }

    // 2. Check for at least one final state
    if (finalStates.empty()) {
        validationErrors.push_back("No final states defined");
        throw MooreMachineValidationException("No final states defined");
    }

    // 3. Check reachability of all states from the start state
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

    // Check for unreachable states
    std::vector<std::string> unreachableStates;
    for (const auto& pair : states) {
        if (!visited.count(pair.first)) {
            unreachableStates.push_back(pair.first);
        }
    }
    
    if (!unreachableStates.empty()) {
        std::string errorMsg = "The following states are unreachable from the start state: ";
        for (size_t i = 0; i < unreachableStates.size(); ++i) {
            if (i > 0) errorMsg += ", ";
            errorMsg += unreachableStates[i];
        }
        validationErrors.push_back(errorMsg);
        throw MooreMachineValidationException(errorMsg);
    }

    // 4. Check determinism (no duplicate input symbols for transitions from the same state)
    for (const auto& pair : states) {
        const auto& state = pair.second;
        std::unordered_map<char, std::string> inputToDestination;  // Change from string to char
        
        for (const auto& dep : state->getDependencies()) {
            char input = dep->getInput();  // Use getInput() which returns a char
            
            // Convert to string for error message purposes only
            std::string inputStr(1, input);
            
            // Check for duplicate transitions with same input
            if (inputToDestination.find(input) != inputToDestination.end()) {
                std::string errorMsg = "State '" + state->getName() + 
                                    "' has multiple transitions for input '" + inputStr + 
                                    "', which violates determinism";
                validationErrors.push_back(errorMsg);
                throw DeterminismViolationException(errorMsg);
            }
            
            // Find the destination state for this transition
            for (const auto& nextState : state->getNextStates()) {
                if (nextState && dep->getFromState() == state) {
                    inputToDestination[input] = nextState->getName();
                    break;
                }
            }
        }
}
    
    // 6. Check for dead-end states (non-final states with no outgoing transitions)
    for (const auto& pair : states) {
        const auto& state = pair.second;
        
        // Check if state has any outgoing transitions
        if (state->getDependencies().empty()) {
            std::string errorMsg = "State '" + state->getName() + 
                                  "' is a non-final state with no outgoing transitions (dead-end)";
            validationErrors.push_back(errorMsg);
            throw MooreMachineValidationException(errorMsg);
        }
    }
    
    // If this comment is reached, the FSM is valid
    std::cout << "FSM validation successful!" << std::endl;
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