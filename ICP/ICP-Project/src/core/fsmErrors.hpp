#ifndef FSM_ERRORS_HPP
#define FSM_ERRORS_HPP

#include <exception>
#include <string>

/**
 * @class FSMException
 * @brief Base class for all FSM exceptions.
 * 
 * Provides a common interface for FSM-related errors.
 */
class FSMException : public std::exception {
protected:
    std::string message; ///< Error message.

public:
    /**
     * @brief Constructor for FSMException.
     * @param msg The error message.
     */
    explicit FSMException(const std::string& msg) : message(msg) {}

    /**
     * @brief Returns the error message.
     * @return A C-style string with the error message.
     */
    const char* what() const noexcept override {
        return message.c_str();
    }
};

/**
 * @class InvalidStateException
 * @brief Thrown when a state is invalid or does not exist.
 */
class InvalidStateException : public FSMException {
public:
    /**
     * @brief Constructor for InvalidStateException.
     * @param stateName The name of the invalid state.
     */
    explicit InvalidStateException(const std::string& stateName)
        : FSMException("Invalid State: " + stateName) {}
};

/**
 * @class InvalidTransitionException
 * @brief Thrown when a transition is invalid or undefined.
 */
class InvalidTransitionException : public FSMException {
public:
    /**
     * @brief Constructor for InvalidTransitionException.
     * @param transitionDetails Details about the invalid transition.
     */
    explicit InvalidTransitionException(const std::string& transitionDetails)
        : FSMException("Invalid Transition: " + transitionDetails) {}
};

/**
 * @class MooreMachineValidationException
 * @brief Thrown when the FSM violates Moore machine rules.
 */
class MooreMachineValidationException : public FSMException {
public:
    /**
     * @brief Constructor for MooreMachineValidationException.
     * @param details Details about the validation error.
     */
    explicit MooreMachineValidationException(const std::string& details)
        : FSMException("Moore Machine Validation Error: " + details) {}
};

/**
 * @class DeterminismViolationException
 * @brief Thrown when the FSM violates determinism rules.
 */
class DeterminismViolationException : public FSMException {
public:
    /**
     * @brief Constructor for DeterminismViolationException.
     * @param details Details about the determinism violation.
     */
    explicit DeterminismViolationException(const std::string& details)
        : FSMException("Determinism Violation: " + details) {}
};

/**
 * @class InvalidInputException
 * @brief Thrown when the FSM receives invalid input.
 */
class InvalidInputException : public FSMException {
public:
    /**
     * @brief Constructor for InvalidInputException.
     * @param input Details about the invalid input.
     */
    explicit InvalidInputException(const std::string& input)
        : FSMException("Invalid Input: " + input) {}
};

// Exception for invalid arguments
class InvalidArgumentException : public FSMException {
    public:
        explicit InvalidArgumentException(const std::string& input)
            : FSMException("Invalid Argument: " + input) {}
};

class InvalidScriptException : public FSMException {
    public:
        explicit InvalidScriptException(const std::string& details)
            : FSMException("Invalid JavaScript: " + details) {}
    };

#endif // FSM_ERRORS_HPP