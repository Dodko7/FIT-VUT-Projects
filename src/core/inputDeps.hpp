#ifndef INPUTDEPS_HPP
#define INPUTDEPS_HPP

#include "state.hpp"
#include <memory> // For smart pointers

class inputDeps {
private:
    char expectedInput; // The input symbol required for the transition
    std::weak_ptr<State> fromState; // The originating state for the transition

public:
    // Constructor
    inputDeps(char input, std::shared_ptr<State> state);

    // Destructor
    ~inputDeps() = default; // Smart pointers handle memory management

    // Get the expected input symbol for the transition
    char getExpectedInput() const;

    // Set the expected input symbol for the transition
    void setExpectedInput(char input);

    // Get the originating state for the transition
    std::shared_ptr<State> getFromState() const;

    // Set the originating state for the transition
    void setFromState(std::shared_ptr<State> state);
};

#endif // INPUTDEPS_HPP