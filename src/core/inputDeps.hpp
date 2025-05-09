#ifndef INPUTDEPS_HPP
#define INPUTDEPS_HPP

#include <memory> // For smart pointers

// Forward declaration of the State class to avoid circular dependency
class State;

/**
 * @class inputDeps
 * @brief Represents a transition dependency based on input and originating state.
 */
class inputDeps {
private:
    char expectedInput; ///< The input symbol required for the transition.
    std::weak_ptr<State> fromState; ///< The originating state for the transition.

public:
    /**
     * @brief Constructor for inputDeps.
     * @param input The input symbol required for the transition.
     * @param state The originating state for the transition.
     */
    inputDeps(char input, std::shared_ptr<State> state);

    /**
     * @brief Default destructor.
     * 
     * Smart pointers handle memory management automatically.
     */
    ~inputDeps() = default;

    /**
     * @brief Gets the expected input symbol for the transition.
     * @return The input symbol.
     */
    char getExpectedInput() const;

    /**
     * @brief Sets the expected input symbol for the transition.
     * @param input The new input symbol.
     */
    void setExpectedInput(char input);

    /**
     * @brief Gets the originating state for the transition.
     * @return A shared pointer to the originating state.
     */
    std::shared_ptr<State> getFromState() const;

    /**
     * @brief Sets the originating state for the transition.
     * @param state A shared pointer to the new originating state.
     */
    void setFromState(std::shared_ptr<State> state);
};

#endif // INPUTDEPS_HPP