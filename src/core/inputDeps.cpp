#include "state.hpp" // Include State to interact with state objects
#include "fsmErrors.hpp" // Include FSM errors for exception handling
#include <memory> // For smart pointers
#include <stdexcept> // For std::invalid_argument

// Class to represent input dependencies for FSM transitions
class inputDeps {
private:
    char expectedInput; // Consider using std::string if inputs can be more complex
    std::weak_ptr<State> fromState; // Consider std::shared_ptr<State> for better memory management

public:
    // Constructor to initialize inputDeps with input and originating state
    inputDeps(char input, std::shared_ptr<State> state) : expectedInput(input), fromState(state) {
        if (input == '\0') { // Validate that the input character is not null
            throw std::invalid_argument("Input character cannot be null");
        }
        if (!state) { // Validate that the originating state is not null
            throw std::invalid_argument("State cannot be null");
        }
    }

    // Default destructor as no manual resource management is needed
    ~inputDeps() = default;

    // Getter for the expected input character
    char getExpectedInput() const {
        return expectedInput;
    }

    // Setter for the expected input character
    void setExpectedInput(char input) {
        if (input == '\0') { // Validate that the input character is not null
            throw std::invalid_argument("Input character cannot be null");
        }
        expectedInput = input; // Update the input character
    }

    // Getter for the originating state
    std::shared_ptr<State> getFromState() const {
        return fromState.lock(); // Return a shared pointer to the originating state
    }

    // Setter for the originating state
    void setFromState(std::shared_ptr<State> state) {
        if (!state) { // Validate that the state is not null
            throw std::invalid_argument("State cannot be null");
        }
        fromState = state; // Update the originating state
    }
};