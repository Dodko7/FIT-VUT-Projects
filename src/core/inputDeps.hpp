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
    std::string timeout; ///< Name of timeout variable.
    std::string output; ///< Output for this transition.
    std::weak_ptr<State> fromState; ///< The originating state for the transition.
    char input; ///< The expected input for the transition.

public:
    /**
     * @brief Constructor for inputDeps.
     * @param event The name of the input event.
     * @param condition The JavaScript condition.
     * @param timeout The name of the timeout variable.
     * @param output The output for this transition.
     * @param state The originating state for the transition.
     */
    inputDeps(const std::string& event, const std::string& condition, 
              const std::string& timeout, const std::string& output, std::shared_ptr<State> state);

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
     * @brief Gets the name of the timeout variable.
     * @return The name of the timeout variable.
     */
    std::string getTimeout() const;

    /**
     * @brief Sets the name of the timeout variable.
     * @param timeout The new name of the timeout variable.
     */
    void setTimeout(const std::string& timeout);

    /**
     * @brief Gets the output for this transition.
     * @return The output for this transition.
     */
    std::string getOutput() const;

    /**
     * @brief Sets the output for this transition.
     * @param output The new output for this transition.
     */
    void setOutput(const std::string& output);

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