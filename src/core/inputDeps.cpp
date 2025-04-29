#include "state.hpp"
#include <memory>
#include <stdexcept> // For std::invalid_argument

class inputDeps {
private:
    char expectedInput; // Consider using std::string if inputs can be more complex
    std::weak_ptr<State> fromState; // Consider std::shared_ptr<State> for better memory management

public:
    // Constructor
    inputDeps(char input, std::shared_ptr<State> state) : expectedInput(input), fromState(state) {
        if (input == '\0') {
            throw std::invalid_argument("Input character cannot be null");
        }
        if (!state) {
            throw std::invalid_argument("State cannot be null");
        }
    }

    // Destructor
    ~inputDeps() = default; // Use default destructor as no manual resource management is needed

    // Getter for expectedInput
    char getExpectedInput() const {
        return expectedInput;
    }

    // Setter for expectedInput
    void setExpectedInput(char input) {
        if (input == '\0') {
            throw std::invalid_argument("Input character cannot be null");
        }
        
        expectedInput = input;
    }

    // Getter for fromState
    std::shared_ptr<State> getFromState() const {
        return fromState.lock();
    }

    // Setter for fromState
    void setFromState(std::shared_ptr<State> state) {
        if (!state) {
            throw std::invalid_argument("State cannot be null");
        }
        fromState = state;
    }
};