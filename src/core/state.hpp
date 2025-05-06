#ifndef STATE_HPP
#define STATE_HPP

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it

/**
 * @class State
 * @brief Represents a state in the finite state machine (FSM).
 */
class State {
    private:
        std::string name; ///< Name of the state.
        std::vector<std::shared_ptr<State>> nextStates; ///< List of states reachable from this state.
        bool isFinal; ///< Indicates if this state is a final state.

        std::function<void()> action; ///< Action to execute when entering this state.
        
        std::vector<std::unique_ptr<inputDeps>> dependencies; ///< Transitions (input dependencies) for this state.
        std::string output; ///< Output associated with this state.
        std::weak_ptr<State> previousState; ///< Pointer to the previous state (weak to avoid ownership issues).

    public:
        /**
         * @brief Constructor for the State class.
         * @param name The name of the state.
         * @param isFinal Whether this state is a final state (default is false).
         */
        State(const std::string& name, bool isFinal = false);

        /**
         * @brief Gets the name of the state.
         * @return The name of the state.
         */
        const std::string& getName() const;

        /**
         * @brief Sets the name of the state.
         * @param name The new name of the state.
         */
        void setName(const std::string& name);

        /**
         * @brief Checks if the state is a final state.
         * @return True if the state is final, false otherwise.
         */
        bool getIsFinal() const;

        /**
         * @brief Sets whether the state is a final state.
         * @param isFinal True if the state is final, false otherwise.
         */
        void setIsFinal(bool isFinal);

        /**
         * @brief Adds a transition to another state.
         * @param nextState Pointer to the next state.
         */
        void addNextState(State* nextState);

        /**
         * @brief Removes a transition to another state.
         * @param nextState Pointer to the state to remove.
         */
        void removeNextState(State* nextState);

        /**
         * @brief Sets the action to execute when entering this state.
         * @param action The action to set.
         */
        void setAction(const std::function<void()>& action);

        /**
         * @brief Executes the action associated with this state.
         */
        void executeAction() const;

        /**
         * @brief Gets the list of next states.
         * @return A reference to the vector of next states.
         */
        std::vector<std::shared_ptr<State>>& getNextStates();

        /**
         * @brief Gets the machine state of this state.
         * @return The machine state.
         */
        machineState getMachineState() const;

        /**
         * @brief Sets the machine state of this state.
         * @param state The new machine state.
         */
        void setMachineState(machineState state);

        /**
         * @brief Gets the output associated with this state.
         * @return The output string.
         */
        const std::string& getOutput() const;

        /**
         * @brief Sets the output associated with this state.
         * @param output The new output string.
         */
        void setOutput(const std::string& output);

        /**
         * @brief Gets the dependencies (transitions) for this state.
         * @return A reference to the vector of input dependencies.
         */
        std::vector<std::unique_ptr<inputDeps>>& getDependencies();
};

#endif // STATE_HPP