/**
 * @file fsm.cpp
 * @brief Implementation of the Finite State Machine (FSM) class
 * @author xvalenk00 && xondre16
 * @date May 2025
 * 
 * This file contains the implementation of a Moore-type Finite State Machine,
 * supporting state creation/management, transitions, serialization, validation,
 * execution, and debugging visualization.
 */

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
#include <set> // For set container
#include <map> // For map container

using json = nlohmann::json;

/**
 * @brief Constructs a new FSM instance
 * 
 * Initializes a new FSM with default values (no states, no transitions, IDLE machine state).
 */
FSM::FSM() : startState(nullptr), currentState(nullptr), stepDelay(0), 
             currentMachineState(machineState::IDLE), scriptEngine(*this) {}

void FSM::addState(const std::string& name, const std::string& action, char output, bool isFinal, std::chrono::milliseconds stepDelay) {
    if (states.find(name) != states.end()) {
        throw InvalidStateException("State already exists: " + name);
    }
    if (name.empty() || name.length() > 20) {
        throw InvalidArgumentException("Invalid state name");
    }

    auto state = std::make_shared<State>(
        name,                               // State name
        std::nullopt,                       // Null optional for machine state
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

/**
 * @brief Sets the name of the FSM
 * 
 * @param name The name to set (non-empty, max 20 characters)
 * @throws InvalidArgumentException If the name is empty or too long
 */
void FSM::setName(const std::string& name) {
    if (name.empty()) { // Validate that the name is not empty
        throw InvalidArgumentException("FSM name cannot be empty");
    }
    if (name.length() > 20) { // Validate that the name is not too long
        throw InvalidArgumentException("FSM name too long");
    }
    this->name = name; // Set the FSM name
}

/**
 * @brief Sets the description of the FSM
 * 
 * @param description The description to set (non-empty, max 100 characters)
 * @throws InvalidArgumentException If the description is empty or too long
 */
void FSM::setDescription(const std::string& description) {
    if (description.empty()) { // Validate that the description is not empty
        throw InvalidArgumentException("FSM description cannot be empty");
    }
    if (description.length() > 100) { // Validate that the description is not too long
        throw InvalidArgumentException("FSM description too long");
    }
    this->description = description; // Set the FSM description
}

/**
 * @brief Gets the name of the FSM
 * 
 * @return The name of the FSM
 */
const std::string& FSM::getName() const {
    return name;
}

/**
 * @brief Gets the description of the FSM
 * 
 * @return The description of the FSM
 */
const std::string& FSM::getDescription() const {
    return description;
}

/**
 * @brief Removes a state from the FSM and recursively prunes unreachable children
 * 
 * @param name The name of the state to remove
 */
void FSM::removeState(const std::string& name) {
    deleteStateRecursive(name); // Call helper function to delete state recursively
}

/**
 * @brief Sets the start state of the FSM
 * 
 * @param name The name of the state to set as the start state
 * @throws InvalidStateException If the state does not exist
 */
void FSM::setStartState(const std::string& name) {
    auto it = states.find(name);
    if (it == states.end()) {
        throw InvalidStateException("State does not exist: " + name);
    }
    startState = it->second;
}

/**
 * @brief Adds a transition between two states
 * 
 * @param fromState The name of the source state
 * @param toState The name of the target state
 * @param condition JavaScript condition expression that must evaluate to true for this transition
 * @param input The input character that triggers this transition
 * @throws InvalidStateException If either state does not exist
 * @throws DeterminismViolationException If the source state already has a transition with the same input
 */
void FSM::addTransition(const std::string& fromState, const std::string& toState, 
                        const std::string& condition, const char input) {
    auto from = getStatePtrByName(fromState);
    auto to = getStatePtrByName(toState);
    if (!from || !to) {
        throw InvalidStateException("Invalid state name");
    }
    
    // Check if the destination state already has a dependency with the same input from this state
    for (const auto& dep : to->getDependencies()) {
        if (dep->getInput() == input && dep->getFromState() == from) {
            throw DeterminismViolationException("Duplicate transition for input: " + std::string(1, input));
        }
    }
    
    auto dep = std::make_unique<inputDeps>(condition, from, input);
    // Add dependency to the destination state, not the source state
    to->addDependency(std::move(dep));
    from->addNextState(to);
}

/**
 * @brief Removes a transition between two states
 * 
 * @param fromState The name of the source state
 * @param toState The name of the target state
 * @param input The input character associated with the transition to remove
 * @throws InvalidArgumentException If state names are empty
 * @throws InvalidStateException If states do not exist
 */
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

    // Remove the transition from the 'from' state's nextStates
    from->removeNextStateOccurrences(to);

    // Remove the dependency from the 'to' state that references 'from'
    auto& deps = to->getDependencies();
    deps.erase(
        std::remove_if(deps.begin(), deps.end(), [&](const std::unique_ptr<inputDeps>& dep) {
            return dep->getInput() == input && dep->getFromState() == from;
        }),
        deps.end()
    );
}

/**
 * @brief Checks if a state with the given name exists in the FSM
 * 
 * @param name The name of the state to check
 * @return true If the state exists
 * @return false If the state does not exist
 * @throws InvalidArgumentException If the name is empty
 */
bool FSM::findStateExists(const std::string& name) const {
    if (name.empty()) {
        throw InvalidArgumentException("State name cannot be empty");
    }
    return states.find(name) != states.end();
}

/**
 * @brief Adds an expected input character to the FSM
 * 
 * @param value The input character to add
 * @throws InvalidArgumentException If the input is null or already exists
 */
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

/**
 * @brief Removes an expected input character from the FSM
 * 
 * Silently ignores if the input does not exist.
 * 
 * @param value The input character to remove
 */
void FSM::removeExpectedInput(const char value) {
    expectedInputs.erase(value); // Ignore if input doesn't exist
}

/**
 * @brief Checks if the first character of the input string is a valid expected input
 * 
 * @return true If the input character is valid
 * @return false If the input character is not valid
 */
bool FSM::checkValidInput() {

    char input = this->input[0]; // Get the first character of the input
    return expectedInputs.find(input) != expectedInputs.end();
}

/**
 * @brief Adds an output character to the output string
 * 
 * @param value The output character to add
 * @throws InvalidArgumentException If the output character is null
 */
void FSM::addOutput(const char value) {
    if (value == '\0') {
        throw InvalidArgumentException("Output cannot be null");
    }
    output += std::string(1, value); // Append the output character to the output string
}

/**
 * @brief Adds or updates a variable in the FSM
 * 
 * @param name The name of the variable
 * @param value The value to assign to the variable
 * @param overwrite Whether to overwrite the variable if it already exists
 * @throws InvalidArgumentException If the name is empty or the variable already exists and overwrite is false
 */
void FSM::addVariable(const std::string& name, const std::string& value, bool overwrite) {
    if (name.empty()) {
        throw InvalidArgumentException("Variable name cannot be empty");
    }
    if (!overwrite && variables.find(name) != variables.end()) {
        throw InvalidArgumentException("Variable already exists: " + name);
    }
    variables[name] = value;
}

/**
 * @brief Removes a variable from the FSM
 * 
 * @param name The name of the variable to remove
 */
void FSM::removeVariable(const std::string& name) {
    variables.erase(name);
}

/**
 * @brief Gets all variables in the FSM
 * 
 * @return Reference to the map of variable names to values
 */
const std::unordered_map<std::string, std::string>& FSM::getVariables() const {
    return variables;
}

/**
 * @brief Performs a single transition based on the current input character
 * 
 * This method processes the first character of the input string, finds a matching transition,
 * updates the current state, executes any associated actions, and updates the output.
 * 
 * @throws InvalidStateException If the current state is null
 * @throws InvalidArgumentException If the input string is empty or no matching transition is found
 */
void FSM::transitionToState() {
    if (!currentState) {
        throw InvalidStateException("Current state is null");
    }
    if (input.empty()) {
        this->setCurrentMachineState(machineState::STOPPED); // Stop the FSM if input is empty
        throw InvalidArgumentException("Input string is empty");
    }
    
    // First check if the input is valid
    checkValidInput();
    
    // Then try to find a matching transition
    char inputCharToProcess = this->input[0]; // Get the first character of the input
    for (auto& next : currentState->getNextStates()) {
        // Look for a dependency in the next state that references the current state
        for (auto& dep : next->getDependencies()) {
            if (dep->getInput() == inputCharToProcess && currentState == dep->getFromState()) { // Check if the input matches
                std::cout << "Transitioning from state " << currentState->getName() << " to state " << next->getName() << " by input " << inputCharToProcess << "\n";
                setCurrentState(next); // Transition to the next state
                addOutput(currentState->getOutput()); // Add the output of the new state
                
                if (!currentState->getAction().empty()) { // Check if the state has an action
                    std::cout << "Executing action: " << currentState->getAction() << "\n";
                    scriptEngine.executeAction(currentState->getAction()); // Execute the action
                }

                discardInputChar(); // Discard the processed input character

                if(currentState->getTransitionTo().has_value()) { // Check if the state has a transition to another machine state
                    setCurrentMachineState(currentState->getTransitionTo().value()); // Set the machine state if defined
                }
                return;
            }
        }
    }
    
    // If we get here, no matching transition was found
    throw InvalidArgumentException("Input cannot be processed: no matching transition found for " + std::string(1, input[0]));
}



/**
 * @brief Runs the FSM with the current input string until completion
 * 
 * Processes the entire input string, updating states, executing actions, 
 * and generating output according to the FSM definition.
 * 
 * @throws MooreMachineValidationException If no start state is defined
 * @throws InvalidArgumentException If the input string is empty
 */
void FSM::run() {
    if (!startState) {
        throw MooreMachineValidationException("No start state defined");
    }
    if (input.empty()) {
        throw InvalidArgumentException("No input sequence provided");
    }
    
    try {
        validateFSM();
    } catch (const std::exception& e) {
        std::cerr << "Running the FSM simulation failed: " << e.what() << '\n';
        return;
    }
    
    clearOutput();
    setCurrentState(startState);
    setCurrentMachineState(machineState::RUNNING);
    addOutput(currentState->getOutput());
    
    // Vykonaj počiatočnú akciu
    if (!currentState->getAction().empty()) {
        std::cout << "Executing initial action: " << currentState->getAction() << "\n";
        scriptEngine.executeAction(currentState->getAction());
    }
    
    std::cout << "Starting FSM at state: " << currentState->getName() << "\n";
    auto start = std::chrono::steady_clock::now();
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
    // Measure run time
    auto stop = std::chrono::steady_clock::now();
    runTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "FSM run time: " << runTime.count() << " ms\n";

    if (currentMachineState == machineState::RUNNING) {
        setCurrentMachineState(machineState::STOPPED);
    }
    
    std::cout << "FSM stopped at state: " << currentState->getName() << "\n";
}

/**
 * @brief Executes a single step of the FSM for debugging purposes
 * 
 * Processes one input character, updating the state and output.
 * 
 * @return true If a step was successfully executed
 * @return false If no more steps can be executed (no more input or error occurred)
 * @throws MooreMachineValidationException If no start state is defined
 */
bool FSM::debugStep() {
    if (!startState) {
        throw MooreMachineValidationException("No start state defined");
    }
    if (input.empty()) {
        setCurrentMachineState(machineState::STOPPED);
        std::cout << "Debug: No more input to process.\n";
        return false;
    }

    try {
        // If the FSM is not already running, initialize it
        if (currentMachineState != machineState::RUNNING) {
            clearOutput();
            setCurrentState(startState);
            setCurrentMachineState(machineState::RUNNING);
            addOutput(currentState->getOutput());

            if (!currentState->getAction().empty()) {
                std::cout << "Executing initial action: " << currentState->getAction() << "\n";
                scriptEngine.executeAction(currentState->getAction());
            }

            std::cout << "Debug: Starting FSM at state: " << currentState->getName() << "\n";
        }

        // Perform a single transition step
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
    } catch (const std::exception& e) {
        std::cerr << "Error during debug step: " << e.what() << "\n";
        setCurrentMachineState(machineState::ERROR);
        return false;
    }
}

/**
 * @brief Generates a Graphviz DOT file visualization of the FSM
 * 
 * Creates a DOT file representation of the FSM structure including states,
 * transitions, and outputs. The file is saved as "assets/fsm_debug.dot".
 */
void FSM::debug() {
    /**
     * @brief Generates a Graphviz DOT file representing the FSM.
     * Uses the internal state of the FSM to ensure consistency with the automaton's specification.
     * This implementation is specifically for Moore machines, where outputs are associated with states.
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

    // Add states with their output (Moore machine)
    for (const auto& pair : states) {
        const auto& state = pair.second;
        
        // Create state label with output character
        std::string stateLabel = state->getName();
        if (state->getOutput() != '\0') {
            stateLabel += "\\nOutput: " + std::string(1, state->getOutput());
        }
        
        // Set attributes for start and final states
        std::string attributes;
        if (state == startState) {
            attributes = "[shape=doublecircle, color=green]";
        } else if (finalStates.count(state->getName())) {
            attributes = "[shape=doublecircle, color=red]";
        }
        
        dotFile << "    \"" << stateLabel << "\" " << attributes << ";\n";
    }

    // Map to store unique transitions by combining from and to states with input
    std::map<std::string, std::string> uniqueTransitions; // Key: fromState|toState, Value: label

    // Process all transitions
    for (const auto& pair : states) {
        const auto& state = pair.second;
        const auto& deps = state->getDependencies();
        const auto& nextStates = state->getNextStates();

        // Create labels for each state that include their output (for Moore machine)
        std::string fromStateLabel = state->getName();
        if (state->getOutput() != '\0') {
            fromStateLabel += "\\nOutput: " + std::string(1, state->getOutput());
        }

        // Iterate over dependencies and next states
        for (size_t i = 0; i < deps.size() && i < nextStates.size(); ++i) {
            const auto& dep = deps[i];
            const auto& nextState = nextStates[i];
            
            // Skip invalid transitions
            if (!nextState || dep->getFromState() != state) {
                continue;
            }

            // Create the destination state label with output
            std::string toStateLabel = nextState->getName();
            if (nextState->getOutput() != '\0') {
                toStateLabel += "\\nOutput: " + std::string(1, nextState->getOutput());
            }

            // Use input character as transition label
            char inputChar = dep->getInput();
            std::string inputLabel = std::string(1, inputChar);
            
            // Create a unique key for this transition
            std::string key = fromStateLabel + "|" + toStateLabel;
            
            // Check if we already have a transition between these states
            if (uniqueTransitions.find(key) != uniqueTransitions.end()) {
                // Append the new input to the existing label
                uniqueTransitions[key] += ", " + inputLabel;
            } else {
                // Create a new transition label
                uniqueTransitions[key] = inputLabel;
            }
        }
    }

    // Write transitions to DOT file
    for (const auto& [key, label] : uniqueTransitions) {
        auto pos = key.find('|');
        if (pos == std::string::npos) continue;
        
        std::string from = key.substr(0, pos);
        std::string to = key.substr(pos + 1);
        
        dotFile << "    \"" << from << "\" -> \"" << to 
                << "\" [label=\"" << label << "\"];\n";
    }

    dotFile << "}\n";
    dotFile.close();
    
    std::cout << "FSM graph generated to assets/fsm_debug.dot" << std::endl;
}

/**
 * @brief Gets the current state of the FSM
 * 
 * @return Shared pointer to the current state
 */
std::shared_ptr<State> FSM::getCurrentState() const {
    return currentState;
}

/**
 * @brief Sets the current state of the FSM
 * 
 * @param state The state to set as current
 * @throws InvalidArgumentException If the state is null
 * @throws InvalidStateException If the state does not exist in the FSM
 */
void FSM::setCurrentState(std::shared_ptr<State> state) {
    if (!state) {
        throw InvalidArgumentException("State cannot be null");
    }

    if (states.find(state->getName()) == states.end()) {
        throw InvalidStateException("State does not exist: " + state->getName());
    }

    currentState = state; // Set the current state
    currentStateEntryTime = std::chrono::steady_clock::now();
}

/**
 * @brief Gets all states in the FSM
 * 
 * @return Reference to the map of state names to state objects
 */
const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getStates() const {
    return states;
}

/**
 * @brief Sets the input string for the FSM
 * 
 * @param input The input string to set
 * @throws InvalidArgumentException If the input is too long or contains unexpected characters
 */
void FSM::setInput(const std::string& input) {
    // if (input.empty()) {
    //     throw InvalidArgumentException("Input cannot be empty");
    // }
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

/**
 * @brief Gets the current input string
 * 
 * @return The current input string
 */
std::string FSM::getInput() const {
    return input;
}

/**
 * @brief Removes the first character from the input string
 * 
 * Used after processing a character during state transitions.
 */
void FSM::discardInputChar() {
    if (!input.empty()) {
        input.erase(0, 1); // Remove the first character from the input string
    }
}

/**
 * @brief Gets the current output string
 * 
 * @return The current output string
 */
std::string FSM::getOutput() const {
    return output;
}

/**
 * @brief Clears the output string
 * 
 * Used when resetting or restarting the FSM.
 */
void FSM::clearOutput() {
    output.clear(); // Clear the output string
}

/**
 * @brief Gets the set of expected input characters
 * 
 * @return Set of expected input characters
 */
std::unordered_set<char> FSM::getExpectedInputs() const {
    return expectedInputs;
}

/**
 * @brief Gets the start state of the FSM
 * 
 * @return Shared pointer to the start state
 */

std::shared_ptr<State> FSM::getStartState() const {
    return startState;
}

/**
 * @brief Gets all final states in the FSM
 * 
 * @return Reference to the map of final state names to state objects
 */
const std::unordered_map<std::string, std::shared_ptr<State>>& FSM::getFinalStates() const {
    return finalStates;
}

/**
 * @brief Gets the current machine state
 * 
 * @return The current machine state (IDLE, RUNNING, STOPPED, PAUSED, ERROR)
 */
machineState FSM::getCurrentMachineState() const {
    return currentMachineState;
}

/**
 * @brief Sets the current machine state
 * 
 * @param state The machine state to set
 * @throws InvalidArgumentException If the state is not a valid machine state
 */
void FSM::setCurrentMachineState(machineState state) {
    static const std::unordered_set<machineState> validStates = {
        machineState::IDLE,
        machineState::RUNNING,
        machineState::STOPPED,
        machineState::PAUSED,
        machineState::ERROR
    };
    if (validStates.find(state) == validStates.end()) {
        throw InvalidArgumentException("Invalid machine state");
    }
    currentMachineState = state;
}

/**
 * @brief Saves the FSM to a JSON file
 * 
 * Serializes the entire FSM including states, transitions, variables,
 * and configuration to a JSON file.
 * 
 * @param filename Path to save the JSON file
 * @throws std::runtime_error If the file cannot be opened for writing
 */
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

    // Inputs - store the whole FSM input string
    j["input"] = getInput(); // Use getter to ensure we get latest input

    // Outputs
    j["output"] = output;

    // Expected inputs
    j["expectedInputs"] = json::array();
    for (const auto& input : expectedInputs) {
        j["expectedInputs"].push_back(std::string(1, input)); // Save as string to avoid ASCII code issues
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

    // Transitions - ensure we match the format used in loadFromJson
    j["transitions"] = json::array();
    std::set<std::string> seenTransitions;
    
    // Iterate through all states
    for (const auto& statePair : states) {
        // For each state, get its dependencies
        const auto& state = statePair.second;
        const auto& deps = state->getDependencies();
        
        // For each dependency, check if it references another state
        for (const auto& dep : deps) {
            auto fromState = dep->getFromState();
            if (!fromState) {
                continue; // Skip invalid dependencies
            }
            
            // Create unique key for this transition to avoid duplicates
            std::string key = fromState->getName() + "|" + 
                             state->getName() + "|" + 
                             dep->getCondition() + "|" + 
                             std::string(1, dep->getInput());
            
            // Skip if we've already seen this transition
            if (seenTransitions.find(key) != seenTransitions.end()) {
                continue;
            }
            
            seenTransitions.insert(key);
            
            // Create the transition JSON object
            json transition;
            transition["from"] = fromState->getName();
            transition["to"] = state->getName();
            transition["condition"] = dep->getCondition();
            transition["inputChar"] = std::string(1, dep->getInput());
            
            // Add to transitions array
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

/**
 * @brief Loads an FSM from a JSON file
 * 
 * Deserializes an FSM from a JSON file, creating states, transitions,
 * and setting configuration according to the file contents.
 * 
 * @param filename Path to the JSON file to load
 * @throws std::runtime_error If the file cannot be opened or contains invalid JSON
 */
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

    // Load input - properly load the FSM's whole input sequence
    if (j.contains("input")) {
        setInput(j["input"].get<std::string>());
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
            
            // Add state with correct parameter order
            addState(name, action, output, isFinal, stateDelay);
        }
    }

    // Load transitions
    if (j.contains("transitions")) {
        for (const auto& transition : j["transitions"]) {
            try {
                std::string from = transition["from"].get<std::string>();
                std::string to = transition["to"].get<std::string>();
                
                // Handle condition field
                std::string condition = "";
                if (transition.contains("condition")) {
                    condition = transition["condition"].get<std::string>();
                }
                
                // Handle timeout field
                std::string timeout = "";
                if (transition.contains("timeout")) {
                    timeout = transition["timeout"].get<std::string>();
                    // If timeout is used in the condition field, add it there
                    if (!timeout.empty() && timeout != "0" && condition.empty()) {
                        condition = "@ " + timeout;
                    }
                }
                
                // Get input character (default to null if not found)
                char input = '\0';

                // First try to get input from inputChar field (how saveToJson stores it)
                if (transition.contains("inputChar")) {
                    std::string inputStr = transition["inputChar"].get<std::string>();
                    if (!inputStr.empty()) {
                        input = inputStr[0]; // Take only the first character
                    }
                }
                // Fall back to input field for backward compatibility
                else if (transition.contains("input")) {
                    if (transition["input"].is_string()) {
                        std::string inputStr = transition["input"].get<std::string>();
                        if (!inputStr.empty()) {
                            input = inputStr[0]; // Take only the first character
                        }
                    } else if (transition["input"].is_number()) {
                        input = static_cast<char>(transition["input"].get<int>());
                    } else {
                        input = transition["input"].get<char>();
                    }
                }
                
                // Try to add the transition
                try {
                    auto fromState = getStatePtrByName(from);
                    auto toState = getStatePtrByName(to);
                    
                    if (!fromState || !toState) {
                        std::cerr << "Warning: Cannot create transition from " << from << " to " << to 
                                  << " - one or both states don't exist" << std::endl;
                        continue;
                    }
                    
                    // Create a dependency for this transition
                    auto dependency = std::make_unique<inputDeps>(condition, fromState, input);
                    
                    // First, check if the dependency already exists to avoid duplicates
                    bool dependencyExists = false;
                    for (const auto& dep : toState->getDependencies()) {
                        if (dep->getInput() == input && 
                            dep->getCondition() == condition &&
                            dep->getFromState() == fromState) {
                            dependencyExists = true;
                            break;
                        }
                    }
                    
                    if (!dependencyExists) {
                        // Add the dependency to the destination state
                        toState->addDependency(std::move(dependency));
                    }
                    
                    // Check if destination state is already in source state's nextStates
                    bool nextStateExists = false;
                    for (const auto& next : fromState->getNextStates()) {
                        if (next == toState) {
                            nextStateExists = true;
                            break;
                        }
                    }
                    
                    if (!nextStateExists) {
                        // Add destination state to source state's nextStates
                        fromState->addNextState(toState);
                    }
                } catch (const DeterminismViolationException& e) {
                    std::cerr << "Warning: " << e.what() << " (skipped during JSON loading)" << std::endl;
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("Error loading transition: " + std::string(e.what()));
            }
        }
    }

    // Load expected inputs
    if (j.contains("expectedInputs")) {
        for (const auto& input : j["expectedInputs"]) {
            if (input.is_string()) {
                std::string inputStr = input.get<std::string>();
                if (!inputStr.empty()) {
                    addExpectedInput(inputStr[0]);
                }
            } else if (input.is_number()) {
                // Handle the case where it was saved as ASCII code
                addExpectedInput(static_cast<char>(input.get<int>()));
            } else {
                // Fallback for any other format
                try {
                    addExpectedInput(input.get<char>());
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to load expected input: " << e.what() << std::endl;
                }
            }
        }
    }

    // Set start state
    if (j.contains("startState") && !j["startState"].get<std::string>().empty()) {
        setStartState(j["startState"].get<std::string>());
    }

    // The user will call validateFSM() manually after setting input
}

/**
 * @brief Validates the FSM for correctness
 * 
 * Checks that the FSM:
 * 1. Has a defined start state
 * 2. Has at least one final state
 * 3. All states are reachable from the start state
 * 4. Is deterministic (no duplicate transitions for the same input)
 * 5. Has no dead-end non-final states
 * 
 * @throws MooreMachineValidationException For general validation errors
 * @throws DeterminismViolationException If the FSM is not deterministic
 */
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
        std::unordered_set<char> seenInputs;

        for (const auto& nextState : state->getNextStates()) {
            if (!nextState) continue;

            for (const auto& dep : nextState->getDependencies()) {
                if (dep->getFromState() == state) {
                    char input = dep->getInput();

                    // Check for duplicate input symbols
                    if (seenInputs.find(input) != seenInputs.end()) {
                        std::string errorMsg = "State '" + state->getName() +
                                               "' has multiple transitions for input '" +
                                               std::string(1, input) + "', which violates determinism";
                        validationErrors.push_back(errorMsg);
                        throw DeterminismViolationException(errorMsg);
                    }

                    seenInputs.insert(input);
                }
            }
        }
    }
    
    // 6. Check for dead-end states (non-final states with no outgoing transitions)
    for (const auto& pair : states) {
        const auto& state = pair.second;
        
        // Check if state has any outgoing transitions
        if (state->getNextStates().empty() && !state->getIsFinal()) {
            std::string errorMsg = "State '" + state->getName() + 
                                  "' is a non-final state with no outgoing transitions (dead-end)";
            validationErrors.push_back(errorMsg);
            throw MooreMachineValidationException(errorMsg);
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
    if (it == states.end()) {
        throw InvalidArgumentException("State not found for deletion: " + name);
        return;
    }
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

void FSM::setStepDelay(std::chrono::milliseconds delay) {
        stepDelay = delay;
    }
    
std::chrono::milliseconds FSM::getStepDelay() const {
    return stepDelay;
}