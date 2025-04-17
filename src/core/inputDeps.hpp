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

        char getExpectedInput() const;
        void setExpectedInput(char input);

        std::shared_ptr<State> getFromState() const;
        void setFromState(std::shared_ptr<State> state);
};

#endif // INPUTDEPS_HPP