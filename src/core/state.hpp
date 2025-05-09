#ifndef STATE_HPP
#define STATE_HPP

#include "fsm.hpp" // Ensure machineState is known
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "inputDeps.hpp" // Include InputDeps to allow State to interact with it

// Forward declaration of machineState enum
enum class machineState;

class State {
    private:
        std::string name; // Name of the state
        std::vector<std::shared_ptr<State>> nextStates; // Use shared_ptr for better memory management
        bool isFinal; // Whether this state is a final state

        std::function<void()> action; // TBD - action taken upon entering this state - is this needed?
        
        std::vector<std::unique_ptr<inputDeps>> dependencies; // Use unique_ptr for automatic cleanup
        std::string output; // Output associated with this state
        std::shared_ptr<State> previousState; // Use weak_ptr to avoid ownership issues
        machineState transToMachineState; // Transition to machine state

    public:
        State(const std::string& name, machineState transToMachineState, std::vector<std::unique_ptr<inputDeps>> dependencies, const std::string& output, std::vector<std::shared_ptr<State>> nextStates, std::shared_ptr<State> previousState, bool isFinal); // Constructor
        ~State() = default; // Default destructor

        // Get the name of the state
        const std::string& getName() const;

        // Set the name of the state
        void setName(const std::string& name);

        // Check if the state is a final state
        bool getIsFinal() const;

        // Set whether the state is a final state
        void setIsFinal(bool isFinal);

        // Add a transition to another state
        void addNextState(std::shared_ptr<State> nextState);

        // Remove a transition to another state (FO)
        void removeNextStateFO(std::shared_ptr<State> nextState);

        // Remove all occurrences of a transition to another state
        void removeNextStateOccurances(std::shared_ptr<State> nextState);

        // Set the action to execute when entering this state
        void setAction(const std::function<void()>& action);

        // Execute the action associated with this state
        void executeAction() const;

        // Get the list of next states (for GUI integration)
        std::vector<std::shared_ptr<State>>& getNextStates();
        const std::vector<std::shared_ptr<State>>& getNextStates() const;

        // Get the machine state of this state
        machineState getMachineState() const;

        // Set the machine state of this state
        void setMachineState(machineState state);

        // Get the output associated with this state
        const std::string& getOutput() const;

        // Set the output associated with this state
        void setOutput(const std::string& output);

        // Get the dependencies (transitions) for this state
        std::vector<std::unique_ptr<inputDeps>>& getDependencies();
        const std::vector<std::unique_ptr<inputDeps>>& getDependencies() const;

        // Get the previous state
        std::shared_ptr<State> getPreviousState() const;

        // Change the previous state
        void changePreviousState(std::shared_ptr<State> previousState);

        // Add a dependency
        void addDependency(std::unique_ptr<inputDeps> dependency);

        // Get a dependency
        std::unique_ptr<inputDeps> getDependency(char input, std::shared_ptr<State> fromState);

        // Remove a dependency
        void removeDependency(std::unique_ptr<inputDeps> dependency);

        // Get the transition to state
        machineState getTransitionTo() const;
};

#endif // STATE_HPP