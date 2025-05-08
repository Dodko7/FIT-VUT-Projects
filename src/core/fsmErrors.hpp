#ifndef FSM_ERRORS_HPP
#define FSM_ERRORS_HPP

#include <exception>
#include <string>

// Base class for FSM-related exceptions
class FSMException : public std::exception {
protected:
    std::string message; // Protected to allow derived classes to set the message

public:
    explicit FSMException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Exception for invalid states
class InvalidStateException : public FSMException {
public:
    explicit InvalidStateException(const std::string& stateName)
        : FSMException("Invalid State: " + stateName) {}
};

// Exception for invalid transitions
class InvalidTransitionException : public FSMException {
public:
    explicit InvalidTransitionException(const std::string& transitionDetails)
        : FSMException("Invalid Transition: " + transitionDetails) {}
};

// Exception for Moore machine-specific validation errors
class MooreMachineValidationException : public FSMException {
public:
    explicit MooreMachineValidationException(const std::string& details)
        : FSMException("Moore Machine Validation Error: " + details) {}
};

// Exception for determinism violations
class DeterminismViolationException : public FSMException {
public:
    explicit DeterminismViolationException(const std::string& details)
        : FSMException("Determinism Violation: " + details) {}
};

// Exception for invalid input
class InvalidInputException : public FSMException {
public:
    explicit InvalidInputException(const std::string& input)
        : FSMException("Invalid Input: " + input) {}
};

class InvalidArgumentException : public FSMException {
    public:
        explicit InvalidArgumentException(const std::string& input)
            : FSMException("Invalid Argument: " + input) {}
};

#endif // FSM_ERRORS_HPP