#include "state.hpp" // Use the header file instead of including the implementation
#include <memory>

class inputDeps {
private:
    char expectedInput; // Consider using std::string if inputs can be more complex
    std::weak_ptr<State> fromState; // Consider std::shared_ptr<State> for better memory management

public:
    inputDeps(char input, std::shared_ptr<State> state);
    ~inputDeps();

    char getExpectedInput() const;
    void setExpectedInput(char input);

    std::shared_ptr<State> getFromState() const;
    void setFromState(std::shared_ptr<State> state);
};

inputDeps::inputDeps(char input, std::shared_ptr<State> state)
    : expectedInput(input), fromState(state) {}

// Destructor removed as smart pointers handle memory management

char inputDeps::getExpectedInput() const {
    return expectedInput;
}

void inputDeps::setExpectedInput(char input) {
    expectedInput = input;
}

std::shared_ptr<State> inputDeps::getFromState() const {
    return fromState.lock();
}

void inputDeps::setFromState(std::shared_ptr<State> state) {
    fromState = state;
}