#ifndef INPUTDEPS_HPP
#define INPUTDEPS_HPP

#include <memory>
#include <string>

// Forward declaration of the State class to avoid circular dependency
class State;

/**
 * @class inputDeps
 * @brief Represents a transition dependency based on input and originating state.
 */
class inputDeps {
private:
    std::weak_ptr<State> fromState; ///< The originating state for the transition.
    char input; ///< The expected input for the transition.

public:
    /**
     * @brief Constructor for inputDeps.
     * @param state The originating state for the transition.
     * @param input The expected input character for the transition.
     */
    inputDeps(std::shared_ptr<State> state, 
                const char input);

    /**
     * @brief Default destructor.
     * 
     * Smart pointers handle memory management automatically.
     */
    ~inputDeps() = default;

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

    char getInput() const;

    void setInput(char input);
};

#endif // INPUTDEPS_HPP