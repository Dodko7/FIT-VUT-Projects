#ifndef INPUTDEPS_HPP
#define INPUTDEPS_HPP

#include <memory>
#include <string>

// Forward declaration of the State class to avoid circular dependency
class State;

/**
 * @class inputDeps
 * @brief Represents a transition dependency in a Moore finite state machine.
 * 
 * This class encapsulates the conditions necessary for a state transition in a Moore machine.
 * It stores the input character that triggers the transition and maintains a weak reference
 * to the originating state to avoid circular references. Additionally, it can store optional
 * condition expressions used for more complex transition logic.
 */
class inputDeps {
private:
    std::weak_ptr<State> fromState; ///< The originating state for the transition (weak reference to avoid circular dependencies).
    char input;                     ///< The input character that triggers this transition.
    std::string condition;          ///< Optional condition expression that must evaluate to true for the transition to occur.

public:
    /**
     * @brief Constructor for inputDeps.
     * 
     * @param condition The condition expression that must evaluate to true for the transition.
     * @param state The originating state for the transition.
     * @param input The input character that triggers this transition.
     * @throws std::invalid_argument If the state pointer is null.
     */
    inputDeps(const std::string& condition, std::shared_ptr<State> state, 
              const char input);

    /**
     * @brief Default destructor.
     * 
     * Smart pointers handle memory management automatically.
     */
    ~inputDeps() = default;

    /**
     * @brief Gets the condition expression for this transition.
     * 
     * @return The condition expression as a string.
     */
    std::string getCondition() const;

    /**
     * @brief Sets the condition expression for this transition.
     * 
     * @param condition The new condition expression.
     */
    void setCondition(const std::string& condition);

    /**
     * @brief Gets the originating state for the transition.
     * 
     * @return A shared pointer to the originating state, or nullptr if the state has been destroyed.
     */
    std::shared_ptr<State> getFromState() const;

    /**
     * @brief Sets the originating state for the transition.
     * 
     * @param state A shared pointer to the new originating state.
     * @throws std::invalid_argument If the state pointer is null.
     */
    void setFromState(std::shared_ptr<State> state);

    /**
     * @brief Gets the input character that triggers this transition.
     * 
     * @return The input character.
     */
    char getInput() const;

    /**
     * @brief Sets the input character that triggers this transition.
     * 
     * @param input The new input character.
     */
    void setInput(char input);
};

#endif // INPUTDEPS_HPP