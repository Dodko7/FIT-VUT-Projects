#include "inputDeps.hpp"
#include "fsmErrors.hpp"
#include <stdexcept>

inputDeps::inputDeps(const std::string& condition, std::shared_ptr<State> state, 
                     const char input)
    : condition(condition), fromState(state), input(input) {
    if (!state) {
        throw std::invalid_argument("State cannot be null");
    }
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

void inputDeps::setInput(char input) {
    this->input = input;
}

char inputDeps::getInput() const {
    return input;
}

std::string inputDeps::getCondition() const {
    return condition;
}

void inputDeps::setCondition(const std::string& condition) {
    this->condition = condition;
}