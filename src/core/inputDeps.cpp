#include "inputDeps.hpp"
#include "fsmErrors.hpp"
#include <stdexcept>

inputDeps::inputDeps(const std::string& event, const std::string& condition, 
                     const std::string& timeout, const std::string& output, std::shared_ptr<State> state)
    : event(event), condition(condition), timeout(timeout), output(output), fromState(state) {
    if (!state) {
        throw std::invalid_argument("State cannot be null");
    }
}

std::string inputDeps::getEvent() const {
    return event;
}

void inputDeps::setEvent(const std::string& event) {
    this->event = event;
}

std::string inputDeps::getCondition() const {
    return condition;
}

void inputDeps::setCondition(const std::string& condition) {
    this->condition = condition;
}

std::string inputDeps::getTimeout() const {
    return timeout;
}

void inputDeps::setTimeout(const std::string& timeout) {
    this->timeout = timeout;
}

std::string inputDeps::getOutput() const {
    return output;
}

void inputDeps::setOutput(const std::string& output) {
    this->output = output;
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