#ifndef FSM_HPP
#define FSM_HPP

#include "state.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <chrono>
#include "nlohmann/json.hpp"

/**
 * @enum machineState
 * @brief Represents the possible execution states of the FSM.
 */
enum class machineState {
    IDLE,
    RUNNING,
    STOPPED,
    PAUSED,
    ERROR
};

/**
 * @class FSM
 * @brief Represents a Finite State Machine (FSM) with basic functionality.
 */
class FSM {
private:
    std::unordered_map<std::string, std::shared_ptr<State>> states; ///< Map of state names to state objects.
    std::unordered_map<std::string, std::shared_ptr<State>> finalStates; ///< Map of final states.
    std::string input; ///< The input used for transitions.
    std::string output; ///< The output of the FSM.
    std::unordered_map<std::string, std::string> variables; ///< Map of internal variables.
    std::string name; ///< Name of the FSM.
    std::string description; ///< Description of the FSM.
    std::chrono::milliseconds stepDelay; ///< Delay between FSM steps (placeholder for future use).
    std::shared_ptr<State> startState; ///< Pointer to the start state.
    std::shared_ptr<State> currentState; ///< Pointer to the current state.
    machineState currentMachineState; ///< Current execution state.
    std::unordered_set<char> expectedInputs; ///< Set of expected inputs as strings.

public:
    /**
     * @brief Default constructor for FSM.
     */
    FSM();

    /**
     * @brief Adds a new state to the FSM.
     * @param name The name of the state (non-empty, max 20 characters).
     * @param action The action associated with the state.
     * @param output The output associated with the state (Moore machine) - single character.
     * @param isFinal Indicates whether the state is final.
     * @param stepDelay The delay on-entry to this state (default is 0).
     * @throws InvalidStateException If state already exists.
     * @throws std::invalid_argument If name is empty or too long.
     */
    void addState(const std::string& name, const std::string& action, char output, bool isFinal, std::chrono::milliseconds stepDelay = std::chrono::milliseconds(0));

    /**
     * @brief Removes a state from the FSM and its references.
     * @param name The name of the state to remove.
     */
    void removeState(const std::string& name);

    /**
     * @brief Sets the start state of the FSM.
     * @param name The name of the start state.
     * @throws InvalidStateException If state does not exist.
     */
    void setStartState(const std::string& name);

    /**
     * @brief Adds a transition between two states based on input.
     * @param fromState The source state name.
     * @param toState The destination state name.
     * @param input The expected input for the transition.
     * @throws InvalidStateException If states do not exist.
     */
    void addTransition(const std::string& fromState, const std::string& toState, const char input);

    /**
     * @brief Removes a transition between two states for a specific event.
     * @param fromState The source state name.
     * @param toState The destination state name.
     * @param input The expected input for the transition.
     * @throws InvalidStateException If states do not exist.
     */
    void removeTransition(std::string& fromState, std::string& toState, char input);

    /**
     * @brief Checks if a state exists in the FSM.
     * @param name The name of the state to check.
     * @return True if the state exists, false otherwise.
     */
    bool findStateExists(const std::string& name) const;

    /**
     * @brief Adds an input to the FSM.
     * @param value The input string (non-empty).
     * @throws std::invalid_argument if input is empty or already exists.
     */
    void addExpectedInput(const char value);

    /**
     * @brief Removes an input from the FSM.
     * @param value The input string to remove.
     */
    void removeExpectedInput(const char value);

    /**
     * @brief Checks if the input is valid.
     * @return True if the input is valid, false otherwise.
     */
    bool checkValidInput();

    /**
     * @brief Adds an output to the FSM.
     * @param value The output character (non-empty).
     * @throws std::invalid_argument If the value is empty or already exists.
     */
    void addOutput(const char value);

    /**
     * @brief Removes an output from the FSM.
     */
    void clearOutput();

    /**
     * @brief Adds a new variable to the FSM.
     * @param name The name of the variable (non-empty).
     * @param value The initial value of the variable.
     * @throws std::invalid_argument If name is empty or variable already exists.
     */
    void addVariable(const std::string& name, const std::string& value);

    /**
     * @brief Removes a variable from the FSM.
     * @param name The name of the variable to remove.
     */
    void removeVariable(const std::string& name);

    /**
     * @brief Makes a single transition to the next state based on the current input.
     */
    void transitionToState();

    /**
     * @brief Runs the FSM using the stored input string.
     * @throws MooreMachineValidationException If no start state is defined.
     * @throws std::invalid_argument If input string is empty.
     */
    void run();

    /**
     * @brief Debug step function - processes the next transition or stops if no more input.
     * @return True if the step was successful, false otherwise.
     */
    bool debugStep();

    /**
     * @brief Generates a Graphviz DOT file for debugging.
     */
    void debug();

    /**
     * @brief Gets the current state of the FSM.
     * @return A shared pointer to the current state.
     */
    std::shared_ptr<State> getCurrentState() const;

    /**
     * @brief Gets the current state of the FSM.
     * @return A shared pointer to the current state.
     */
    void setCurrentState(std::shared_ptr<State> state);

    /**
     * @brief Gets all states in the FSM.
     * @return A reference to the map of states.
     */
    const std::unordered_map<std::string, std::shared_ptr<State>>& getStates() const;

    /**
     * @brief Sets the input string of the FSM.
     * @param input The input string to set.
     */
    void setInput(const std::string& input);

    /**
     * @brief Gets the input string of the FSM.
     * @return The current input string.
     */
    std::string getInput() const;

    /**
     * @brief Discards the first character of the input string.
     */
    void discardInputChar();

    /**
     * @brief Gets the output of the FSM.
     * @return The current output string.
     */
    std::string getOutput() const;

    /**
     * @brief Gets the expected inputs of the FSM.
     * @return A set of expected input strings.
     */
    std::unordered_set<char> getExpectedInputs() const;

    /**
     * @brief Gets all variables in the FSM.
     * @return A reference to the map of variables.
     */
    const std::unordered_map<std::string, std::string>& getVariables() const;

    /**
     * @brief Gets the start state of the FSM.
     * @return A shared pointer to the start state.
     */
    std::shared_ptr<State> getStartState() const;

    /**
     * @brief Gets all final states in the FSM.
     * @return A reference to the map of final states.
     */
    const std::unordered_map<std::string, std::shared_ptr<State>>& getFinalStates() const;

    /**
     * @brief Validates the FSM for determinism and reachability.
     */
    void validateFSM();

    /**
     * @brief Saves the FSM to a JSON file.
     * @param filename The name of the file to save to.
     */
    void saveToJson(const std::string& filename);

    /**
     * @brief Loads the FSM from a JSON file.
     * @param filename The name of the file to load from.
     */
    void loadFromJson(const std::string& filename);

    /**
     * @brief Gets a state by its name.
     * @param name The name of the state.
     * @return A shared pointer to the state, or nullptr if not found.
     */
    std::shared_ptr<State> getStatePtrByName(const std::string& name);

    /**
     * @brief Gets all state names in the FSM.
     * @return A vector of state names.
     */
    std::vector<std::string> getAllStateNames() const;

    /**
     * @brief Removes references to a state from other states.
     * @param stateName The name of the state to remove references to.
     */
    void removeReferencesToState(const std::string& stateName);

    /**
     * @brief Recursively deletes a state and its unreachable children.
     * @param name The name of the state to delete.
     */
    void deleteStateRecursive(const std::string& name);

    /**
     * @brief Gets the current machine state.
     * @return The current machine state.
     */
    machineState getCurrentMachineState() const;

    /**
     * @brief Sets the current machine state.
     * @param state The new machine state.
     */
    void setCurrentMachineState(machineState state);

    /**
     * @brief Sets the name of the FSM.
     * @param name The name to set (non-empty, max 20 characters).
     * @throws std::invalid_argument If name is empty or too long.
     */
    void setName(const std::string& name);

    /**
     * @brief Sets the description of the FSM.
     * @param description The description to set (non-empty, max 100 characters).
     * @throws std::invalid_argument If description is empty or too long.
     */
    void setDescription(const std::string& description);

    /**
     * @brief Prunes unreachable states from the FSM.
     */
    void pruneUnreachable();

    /**
     * @brief Gets the name of the FSM.
     * @return The name of the FSM.
     */
    const std::string& getName() const;

    /**
     * @brief Gets the description of the FSM.
     * @return The description of the FSM.
     */
    const std::string& getDescription() const;

private:
    /**
     * @brief Checks if a state is referenced elsewhere.
     * @param stateName The name of the state to check.
     * @param parentName The name of the parent state to exclude.
     * @return True if the state is referenced, false otherwise.
     */
    bool isStateReferencedElsewhere(const std::string& stateName, const std::string& parentName);

    /**
     * @brief Prunes unreachable states recursively.
     * @param state The current state.
     * @param visited Set of visited state names.
     * @param parentName The parent state name.
     */
    void pruneUnreachableStates(const std::shared_ptr<State>& state, std::unordered_set<std::string>& visited, const std::string& parentName);
};

#endif // FSM_HPP