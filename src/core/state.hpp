#ifndef STATE_HPP
#define STATE_HPP

#include "fsm.hpp" // Ensure machineState is known
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "inputDeps.hpp"

/**
 * @class State
 * @brief Represents a state in a Moore finite state machine.
 * 
 * This class encapsulates a state in a Moore machine, which associates outputs with states
 * rather than transitions. It contains information about state transitions, dependencies,
 * output, and other state-specific properties.
 */
class State {
private:
    std::string name;                                    ///< The unique name identifier for this state
    std::vector<std::shared_ptr<State>> nextStates;      ///< Collection of possible next states
    bool isFinal;                                        ///< Flag indicating if this is a final/accepting state
    std::vector<std::unique_ptr<inputDeps>> dependencies; ///< Input dependencies that trigger transitions
    std::chrono::milliseconds stepDelay;                 ///< Delay applied when entering this state
    std::string action;                                  ///< JavaScript code executed when in this state
    char output;                                         ///< Output character associated with this state (Moore machine)
    std::optional<machineState> transToMachineState;     ///< Optional transition to a different machine state

public:
    /**
     * @brief Constructor for a State object.
     * 
     * @param name The unique name identifier for this state
     * @param transToMachineState Optional transition to a different machine state
     * @param dependencies Collection of input dependencies for transitions
     * @param action JavaScript code to execute when in this state
     * @param output Output character produced by this state (Moore machine)
     * @param stepDelay Time delay applied when entering this state
     * @param nextStates Collection of possible next states
     * @param isFinal Flag indicating if this is a final/accepting state
     * @throws std::invalid_argument If name is empty or longer than 20 characters
     */
    State(const std::string& name, std::optional<machineState> transToMachineState, 
          std::vector<std::unique_ptr<inputDeps>> dependencies, 
          const std::string& action,
          char output,
          std::chrono::milliseconds stepDelay, 
          std::vector<std::shared_ptr<State>> nextStates, 
          bool isFinal);
    /**
     * @brief Default destructor.
     */
    ~State() = default;
    
    /**
     * @brief Gets the name of this state.
     * @return The name of the state as a const string reference.
     */
    const std::string& getName() const;

    /**
     * @brief Sets the name of this state.
     * @param name The new name for the state.
     * @throws std::invalid_argument If name is empty or longer than 20 characters.
     */
    void setName(const std::string& name);

    /**
     * @brief Checks if this state is a final/accepting state.
     * @return True if this is a final state, false otherwise.
     */
    bool getIsFinal() const;

    /**
     * @brief Sets whether this state is a final/accepting state.
     * @param isFinal Boolean indicating if this state should be final.
     */
    void setIsFinal(bool isFinal);

    /**
     * @brief Adds a dependency to this state's transitions.
     * @param dependency Unique pointer to an inputDeps object defining a transition condition.
     * @note If the dependency already exists, it will not be added and a warning will be printed.
     */
    void addDependency(std::unique_ptr<inputDeps> dependency);

    /**
     * @brief Gets a dependency that matches the specified input and source state.
     * @param input The input character that triggers this dependency.
     * @param fromState The source state of the transition.
     * @return A unique pointer to the matching dependency, or nullptr if not found.
     * @note If no matching dependency is found, a warning is printed to stderr.
     */
    std::unique_ptr<inputDeps> getDependency(char input, std::shared_ptr<State> fromState);

    /**
     * @brief Removes a dependency from this state's transitions.
     * @param dependency Unique pointer to the inputDeps object to remove.
     * @note If the dependency is not found, a warning is printed to stderr.
     */
    void removeDependency(std::unique_ptr<inputDeps> dependency);

    /**
     * @brief Gets all dependencies for this state.
     * @return A reference to the vector of dependency objects.
     */
    std::vector<std::unique_ptr<inputDeps>>& getDependencies();

    /**
     * @brief Adds a state to the list of possible next states.
     * @param nextState Shared pointer to the state to add.
     * @throws std::invalid_argument If nextState is null.
     * @note Duplicate next states are allowed, as determinism is checked in FSM::addTransition.
     */
    void addNextState(std::shared_ptr<State> nextState);

    /**
     * @brief Removes the first occurrence of a state from the next states list.
     * @param nextState Shared pointer to the state to remove.
     * @throws std::invalid_argument If nextState is null.
     * @note If the state is not found, a warning is printed to stderr.
     */
    void removeNextStateFirstOccurence(std::shared_ptr<State> nextState);

    /**
     * @brief Removes all occurrences of a state from the next states list.
     * @param nextState Shared pointer to the state to remove.
     * @throws std::invalid_argument If nextState is null.
     * @note If the state is not found, a warning is printed to stderr.
     */
    void removeNextStateOccurences(std::shared_ptr<State> nextState);

    /**
     * @brief Gets the JavaScript action code for this state.
     * @return The action code as a string.
     */
    const std::string& getAction() const;

    /**
     * @brief Sets the JavaScript action code for this state.
     * @param action The new action code.
     */
    void setAction(const std::string& action);

    /**
     * @brief Gets the output character for this state (Moore machine).
     * @return The output character.
     */
    char getOutput() const;

    /**
     * @brief Sets the output character for this state (Moore machine).
     * @param output The new output character.
     */
    void setOutput(char output);

    /**
     * @brief Gets all possible next states from this state.
     * @return A reference to the vector of next states.
     */
    std::vector<std::shared_ptr<State>>& getNextStates();

    /**
     * @brief Gets all possible next states from this state (const version).
     * @return A const reference to the vector of next states.
     */
    const std::vector<std::shared_ptr<State>>& getNextStates() const;
    
    /**
     * @brief Gets the delay applied when entering this state.
     * @return Time delay in milliseconds.
     */
    std::chrono::milliseconds getStepDelay() const;
    
    /**
     * @brief Gets the optional transition to another machine state.
     * @return An optional containing the target machine state, or nullopt if no transition.
     */
    std::optional<machineState> getTransitionTo() const;

    /**
     * @brief Sets the target machine state for an automatic transition.
     * @param transToMachineState The target machine state.
     */
    void setTransitionTo(machineState transToMachineState);

    /**
     * @brief Clears any automatic transition to another machine state.
     */
    void clearTransitionTo();
};

#endif // STATE_HPP