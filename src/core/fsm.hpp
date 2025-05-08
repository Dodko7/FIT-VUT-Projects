#ifndef FSM_HPP
#define FSM_HPP

#include "state.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <chrono>

/**
 * @enum machineState
 * @brief Represents the possible execution states of the FSM.
 */
enum class machineState {
    IDLE,
    RUNNING,
    STOPPED,
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
    std::unordered_map<std::string, std::string> inputs; ///< Map of input names to their last values.
    std::unordered_map<std::string, std::string> outputs; ///< Map of output names to their last values.
    std::string name; ///< Name of the FSM.
    std::string description; ///< Description of the FSM.
    std::chrono::milliseconds stepDelay; ///< Delay between FSM steps (placeholder for future use).
    std::deque<char> input; ///< Input sequence (placeholder for future use).
    std::string output; ///< Output sequence (placeholder for future use).
    std::shared_ptr<State> startState; ///< Pointer to the start state.
    std::shared_ptr<State> currentState; ///< Pointer to the current state.
    machineState currentMachineState; ///< Current execution state.
    std::chrono::milliseconds fsmRunTime; ///< Total runtime (placeholder for future use).
    std::unordered_map<std::string, char> allowedInputs; ///< Allowed inputs (placeholder for future use).

public:
    /**
     * @brief Default constructor for FSM.
     */
    FSM();

    /**
     * @brief Adds a new state to the FSM.
     * @param name The name of the state (non-empty, max 20 characters).
     * @param description The description of the state (non-empty).
     * @param isFinal Indicates whether the state is final.
     * @throws InvalidStateException If state already exists.
     * @throws std::invalid_argument If name is empty or too long.
     */
    void addState(const std::string& name, const std::string& description, bool isFinal);

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
     * @brief Adds a transition between two states.
     * @param fromState The source state name.
     * @param toState The destination state name.
     * @param input The input symbol triggering the transition.
     * @throws InvalidStateException If states do not exist.
     * @throws InvalidInputException If input is null.
     * @throws DeterminismViolationException If transition violates determinism.
     */
    void addTransition(std::string& fromState, std::string& toState, char input);

    /**
     * @brief Removes a transition between two states for a specific input.
     * @param fromState The source state name.
     * @param toState The destination state name.
     * @param input The input symbol of the transition to remove.
     */
    void removeTransition(const std::string& fromState, const std::string& toState, char input);

    /**
     * @brief Adds a new input to the FSM.
     * @param name The name of the input (non-empty).
     * @param value The initial value of the input.
     * @throws std::invalid_argument If name is empty or input already exists.
     */
    void addInput(const std::string& name, const std::string& value);

    /**
     * @brief Removes an input from the FSM.
     * @param name The name of the input to remove.
     */
    void removeInput(const std::string& name);

    /**
     * @brief Adds a new output to the FSM.
     * @param name The name of the output (non-empty).
     * @param value The initial value of the output.
     * @throws std::invalid_argument If name is empty or output already exists.
     */
    void addOutput(const std::string& name, const std::string& value);

    /**
     * @brief Removes an output from the FSM.
     * @param name The name of the output to remove.
     */
    void removeOutput(const std::string& name);

    /**
     * @brief Runs the FSM with a given input sequence.
     * @param inputSequence The sequence of inputs to process.
     * @throws MooreMachineValidationException If no start state is defined.
     */
    void run(const std::string& inputSequence);

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
     * @brief Gets all states in the FSM.
     * @return A reference to the map of states.
     */
    const std::unordered_map<std::string, std::shared_ptr<State>>& getStates() const;

    /**
     * @brief Gets all inputs in the FSM.
     * @return A reference to the map of inputs.
     */
    const std::unordered_map<std::string, std::string>& getInputs() const;

    /**
     * @brief Gets all outputs in the FSM.
     * @return A reference to the map of outputs.
     */
    const std::unordered_map<std::string, std::string>& getOutputs() const;

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
     * @brief Saves the FSM to a JSON file (placeholder for future implementation).
     * @param filename The name of the file to save to.
     */
    void saveToJson(const std::string& filename);

    /**
     * @brief Loads the FSM from a JSON file (placeholder for future implementation).
     * @param filename The name of the file to load from.
     */
    void loadFromJson(const std::string& filename);

    /**
     * @brief Gets a state by its name.
     * @param name The name of the state.
     * @return A shared pointer to the state, or nullptr if not found.
     */
    std::shared_ptr<State> getStatePtrByName(std::string& name);

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

    /**
     * @brief Public method to prune unreachable states.
     */
    void pruneUnreachable();
};

#endif // FSM_HPP