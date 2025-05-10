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
    std::string event; ///< Name of the event on-transition.
    std::string condition; ///< JavaScript condition.
    std::weak_ptr<State> fromState; ///< The originating state for the transition.
    char input; ///< The expected input for the transition.

public:
    /**
     * @brief Constructor for inputDeps.
     * @param event The name of the input event.
     * @param condition The JavaScript condition.
     * @param state The originating state for the transition.
     * @param input The expected input character for the transition.
     */
    inputDeps(const std::string& event, const std::string& condition, 
                std::shared_ptr<State> state, 
                const char input);

    /**
     * @brief Default destructor.
     * 
     * Smart pointers handle memory management automatically.
     */
    ~inputDeps() = default;

    /**
     * @brief Gets the name of the input event.
     * @return The name of the input event.
     */
    std::string getEvent() const;

    /**
     * @brief Sets the name of the input event.
     * @param event The new name of the input event.
     */
    void setEvent(const std::string& event);

    /**
     * @brief Gets the JavaScript condition.
     * @return The JavaScript condition.
     */
    std::string getCondition() const;

    /**
     * @brief Sets the JavaScript condition.
     * @param condition The new JavaScript condition.
     */
    void setCondition(const std::string& condition);

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