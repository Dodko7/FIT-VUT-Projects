#ifndef FSM_INTERFACE_HPP
#define FSM_INTERFACE_HPP

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <optional>
#include <nlohmann/json.hpp>
#include "fsm.hpp"
#include "state.hpp"
#include "inputDeps.hpp"

/**
 * @class FSMManager
 * @brief Interface layer between the core FSM library and UI components.
 * 
 * This class provides a simplified interface for creating, manipulating,
 * and executing FSM objects. It handles serialization/deserialization,
 * state management, transition management, and execution control.
 */
class FSMManager {
private:
    std::shared_ptr<FSM> currentFsm; ///< The currently active FSM instance
    std::string lastError; ///< The last error message
    std::string previousState; ///< Name of the previous state for transition tracking

public:
    /**
     * @brief Constructor for FSMManager
     */
    FSMManager();

    /**
     * @brief Destructor for FSMManager
     */
    ~FSMManager() = default;

    /**
     * @brief Creates a new FSM instance
     * @param name The name of the FSM
     * @param description The description of the FSM
     * @return True if the FSM was created successfully, false otherwise
     */
    bool createFSM(const std::string& name, const std::string& description, 
                   const std::chrono::milliseconds stepDelay);

    /**
     * @brief Loads an FSM from a JSON file
     * @param filename The path to the JSON file
     * @return True if the FSM was loaded successfully, false otherwise
     */
    bool loadFSM(const std::string& filename);

    /**
     * @brief Saves the current FSM to a JSON file
     * @param filename The path to save the JSON file
     * @return True if the FSM was saved successfully, false otherwise
     */
    bool saveFSM(const std::string& filename);

    /**
     * @brief Exports the current FSM to a JSON string
     * @return The JSON string representation of the FSM or empty string on error
     */
    std::string exportFSMToJson();

    /**
     * @brief Imports an FSM from a JSON string
     * @param jsonString The JSON string representation of the FSM
     * @return True if the FSM was imported successfully, false otherwise
     */
    bool importFSMFromJson(const std::string& jsonString);

    /**
     * @brief Gets the last error message
     * @return The last error message
     */
    std::string getLastError() const;

    // State Management

    /**
     * @brief Adds a new state to the FSM
     * @param name The name of the state
     * @param action The JavaScript action code
     * @param output The output character
     * @param isFinal Whether the state is final
     * @param stepDelay The delay in milliseconds
     * @return True if the state was added successfully, false otherwise
     */
    bool addState(const std::string& name, const std::string& action, char output, 
                  bool isFinal, std::chrono::milliseconds stepDelay = std::chrono::milliseconds(0));

    /**
     * @brief Removes a state from the FSM
     * @param name The name of the state to remove
     * @return True if the state was removed successfully, false otherwise
     */
    bool removeState(const std::string& name);

    /**
     * @brief Sets the start state of the FSM
     * @param name The name of the start state
     * @return True if the start state was set successfully, false otherwise
     */
    bool setStartState(const std::string& name);

    /**
     * @brief Gets information about a specific state
     * @param name The name of the state
     * @return A JSON object containing state information or null if the state doesn't exist
     */
    nlohmann::json getStateInfo(const std::string& name);

    /**
     * @brief Gets all states in the FSM
     * @return A vector of state names
     */
    std::vector<std::string> getAllStateNames() const;

    /**
     * @brief Updates properties of an existing state
     * @param name The name of the state
     * @param newName The new name for the state (optional, empty string to keep current)
     * @param action The new action code (optional, empty string to keep current)
     * @param output The new output character (optional, '\0' to keep current)
     * @param isFinal The new final state flag (optional)
     * @param updateIsFinal Whether to update the isFinal flag
     * @param stepDelay The new step delay (optional)
     * @param updateStepDelay Whether to update the step delay
     * @return True if the state was updated successfully, false otherwise
     */
    bool updateState(const std::string& name, const std::string& newName, 
                    const std::string& action, char output,
                    bool isFinal, bool updateIsFinal,
                    std::chrono::milliseconds stepDelay, bool updateStepDelay);

    // Transition Management

    /**
     * @brief Adds a transition between two states
     * @param fromState The source state name
     * @param toState The destination state name
     * @param condition The condition expression
     * @param input The input character
     * @return True if the transition was added successfully, false otherwise
     */
    bool addTransition(const std::string& fromState, const std::string& toState, 
                       const std::string& condition, char input);

    /**
     * @brief Removes a transition between two states
     * @param fromState The source state name
     * @param toState The destination state name
     * @param input The input character
     * @return True if the transition was removed successfully, false otherwise
     */
    bool removeTransition(const std::string& fromState, const std::string& toState, char input);

    /**
     * @brief Gets all transitions from a state
     * @param stateName The name of the state
     * @return A JSON array of transitions
     */
    nlohmann::json getStateTransitions(const std::string& stateName);

    /**
     * @brief Updates an existing transition
     * @param fromState The source state name
     * @param toState The destination state name
     * @param oldInput The input character of the transition to update
     * @param newInput The new input character
     * @param newCondition The new condition expression
     * @return True if the transition was updated successfully, false otherwise
     */
    bool updateTransition(const std::string& fromState, const std::string& toState, 
                         char oldInput, char newInput, const std::string& newCondition);

    // Input and Variable Management

    /**
     * @brief Adds an expected input character
     * @param input The input character to add
     * @return True if the input was added successfully, false otherwise
     */
    bool addExpectedInput(char input);

    /**
     * @brief Removes an expected input character
     * @param input The input character to remove
     * @return True if the input was removed successfully, false otherwise
     */
    bool removeExpectedInput(char input);

    /**
     * @brief Gets all expected input characters
     * @return A vector of expected input characters
     */
    std::vector<char> getExpectedInputs() const;

    /**
     * @brief Sets the input string for FSM execution
     * @param input The input string
     * @return True if the input was set successfully, false otherwise
     */
    bool setInput(const std::string& input);

    /**
     * @brief Gets the current input string
     * @return The current input string
     */
    std::string getInput() const;

    /**
     * @brief Gets the current output string
     * @return The current output string
     */
    std::string getOutput() const;

    /**
     * @brief Adds or updates a variable
     * @param name The variable name
     * @param value The variable value
     * @param overwrite Whether to overwrite if the variable already exists
     * @return True if the variable was added/updated successfully, false otherwise
     */
    bool addVariable(const std::string& name, const std::string& value, bool overwrite = false);

    /**
     * @brief Removes a variable
     * @param name The variable name
     * @return True if the variable was removed successfully, false otherwise
     */
    bool removeVariable(const std::string& name);

    /**
     * @brief Gets all variables
     * @return A map of variable names to values
     */
    std::unordered_map<std::string, std::string> getVariables() const;

    // Execution Control

    /**
     * @brief Runs the FSM with the current input
     * @return True if the execution was successful, false otherwise
     */
    bool run();

    /**
     * @brief Takes a single step in the FSM execution
     * @return True if the step was successful, false otherwise (e.g., reached end or error)
     */
    bool step();

    /**
     * @brief Resets the FSM to its initial state
     * @return True if the reset was successful, false otherwise
     */
    bool reset();

    /**
     * @brief Gets the current state of the FSM
     * @return The name of the current state or empty string if no current state
     */
    std::string getCurrentState() const;

    /**
     * @brief Gets the current machine state
     * @return The current machine state (IDLE, RUNNING, etc.)
     */
    machineState getMachineState() const;

    /**
     * @brief Sets the machine state
     * @param state The new machine state
     * @return True if the state was changed successfully, false otherwise
     */
    bool setMachineState(machineState state);

    /**
     * @brief Validates the FSM for correctness
     * @return True if the FSM is valid, false otherwise
     */
    bool validateFSM();

    /**
     * @brief Prunes unreachable states from the FSM
     * @return True if successful, false otherwise
     */
    bool pruneUnreachableStates();

    /**
     * @brief Gets a handle to the underlying FSM object
     * @return Shared pointer to the FSM object
     */
    std::shared_ptr<FSM> getFSM() const;
};

#endif // FSM_INTERFACE_HPP