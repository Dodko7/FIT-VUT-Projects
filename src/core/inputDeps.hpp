#ifndef INPUTDEPS_HPP
#define INPUTDEPS_HPP

#include "state.hpp"
#include <memory> // For smart pointers

class inputDeps {
    private:
        char expectedInput;
        std::weak_ptr<State> fromState; // Use weak_ptr to avoid ownership issues

    public:
        inputDeps(char input, std::shared_ptr<State> state);
        ~inputDeps() = default; // Smart pointers handle memory management

        char getExpectedInput() const {
            return expectedInput;
        }
        void setExpectedInput(char input) {
            expectedInput = input;
        }

        std::shared_ptr<State> getFromState() const{
            return fromState.lock(); // Convert weak_ptr to shared_ptr
        }
        void setFromState(std::shared_ptr<State> state) {
            fromState = state; // Set the weak_ptr to the new state
        }
};

#endif // INPUTDEPS_HPP