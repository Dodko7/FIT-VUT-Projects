#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include "../messages/message_base.hpp"
#include <memory>

/**
 * @class StateMachine
 * @brief Implements a Mealy finite state machine for the IPK25-CHAT client.
 */
class StateMachine {
public:
    // Enum for client states.
    enum class State {
        START,
        AUTH,
        OPEN,
        JOIN,
        END
    };

    /**
     * @brief Constructs a state machine initialized to the START state.
     */
    StateMachine();

    /**
     * @brief Transitions the state machine based on server or client messages.
     * @param server_msg Message from the server.
     * @param client_msg Message from the client.
     * @return New state after transition.
     */
    StateMachine::State transition(const std::shared_ptr<MessageBase>& server_msg, const std::shared_ptr<MessageBase>& client_msg);

    // Returns the current state.
    StateMachine::State get_current_state() const { return current_state_; }

private:
    /**
     * @brief Checks if a message matches the specified type.
     * @param msg The message to check.
     * @param type The expected message type.
     * @return True if the message matches the type, false otherwise.
     */
    bool is_message_type(const std::shared_ptr<MessageBase>& msg, MessageBase::Type type) const;

    StateMachine::State current_state_; // Current state
};

#endif // STATE_MACHINE_HPP