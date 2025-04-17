#ifndef STATE_HPP
#define STATE_HPP

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it

class State {
    private:
        std::string name; // Name of the state
        std::vector<std::shared_ptr<State>> nextStates; // Use shared_ptr for better memory management
        bool isFinal; // Whether this state is a final state
        std::function<void()> action; // TBD - action taken upon entering this state - is this needed?
        machineState currentMachineState; // Current machine state associated with this state
        std::vector<std::unique_ptr<inputDeps>> dependencies; // Use unique_ptr for automatic cleanup
        std::string output; // Output associated with this state
        std::weak_ptr<State> previousState; // Use weak_ptr to avoid ownership issues

    public:
        State(const std::string& name, bool isFinal = false); // Constructor

        // Get the name of the state
        const std::string& getName() const;

        // Set the name of the state
        void setName(const std::string& name);

        // Check if the state is a final state
        bool getIsFinal() const;

        // Set whether the state is a final state
        void setIsFinal(bool isFinal);

        // Add a transition to another state
        void addNextState(State* nextState);

        // Remove a transition to another state
        void removeNextState(State* nextState);

        // Set the action to execute when entering this state
        void setAction(const std::function<void()>& action);

        // Execute the action associated with this state
        void executeAction() const;

        // Get the list of next states (for GUI integration)
        const std::vector<std::shared_ptr<State>>& getNextStates() const;

        // Get the machine state of this state
        machineState getMachineState() const;

        // Set the machine state of this state
        void setMachineState(machineState state);

        // Get the output associated with this state
        const std::string& getOutput() const;

        // Set the output associated with this state
        void setOutput(const std::string& output);
};

#endif // STATE_HPP