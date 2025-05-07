#include "inputDeps.hpp"
#include "fsmErrors.hpp"
#include <stdexcept>

inputDeps::inputDeps(char input, std::shared_ptr<State> state)
    : expectedInput(input), fromState(state) {
    if (input == '\0') {
        throw std::invalid_argument("Input character cannot be null");
    }
    if (!state) {
        throw std::invalid_argument("State cannot be null");
    }
}

char inputDeps::getExpectedInput() const {
    return expectedInput;
}

void inputDeps::setExpectedInput(char input) {
    if (input == '\0') {
        throw std::invalid_argument("Input character cannot be null");
    }
    expectedInput = input;
}

std::shared_ptr<State> inputDeps::getFromState() const {
    return fromState.lock();
}

void inputDeps::setFromState(std::shared_ptr<State> state) {
    if (!state) {
        throw std::invalid_argument("State cannot be null");
    }
    fromState = state;
}