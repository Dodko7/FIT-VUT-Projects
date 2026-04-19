#include "state_machine.hpp"
#include "../common/debug.hpp"
#include <memory>

// Constructs a state machine initialized to the START state.
StateMachine::StateMachine() : current_state_(StateMachine::State::START) {
    printf_debug("StateMachine initialized in START");
}

// Transitions the state machine based on server or client messages.
// Returns the new state.
StateMachine::State StateMachine::transition(const std::shared_ptr<MessageBase>& server_msg,
                                            const std::shared_ptr<MessageBase>& client_msg) {
    printf_debug("Transitioning from state=%d", static_cast<int>(current_state_));
    // Ignore transitions in END state.
    if (current_state_ == StateMachine::State::END) {
        printf_debug("In END state, ignoring transition");
        return current_state_;
    }

    // Handle transitions based on current state.
    switch (current_state_) {
        case StateMachine::State::START:
            if (is_message_type(client_msg, MessageBase::Type::AUTH)) {
                current_state_ = StateMachine::State::AUTH;
            } else if (is_message_type(client_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            } else if (is_message_type(server_msg, MessageBase::Type::ERR) ||
                       is_message_type(server_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            }
            break;
        case StateMachine::State::AUTH:
            if (is_message_type(server_msg, MessageBase::Type::REPLY)) {
                // Transition to OPEN on success, stay in AUTH on failure.
                current_state_ = server_msg->is_success() ? StateMachine::State::OPEN : StateMachine::State::AUTH;
            } else if (is_message_type(server_msg, MessageBase::Type::ERR) ||
                       is_message_type(server_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            } else if (is_message_type(server_msg, MessageBase::Type::MSG)) {
                current_state_ = StateMachine::State::END;
            } else if (is_message_type(client_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            }
            break;
        case StateMachine::State::OPEN:
            if (is_message_type(server_msg, MessageBase::Type::MSG)) {
                current_state_ = StateMachine::State::OPEN;
            } else if (is_message_type(client_msg, MessageBase::Type::MSG)) {
                current_state_ = StateMachine::State::OPEN;
            } else if (is_message_type(client_msg, MessageBase::Type::JOIN)) {
                current_state_ = StateMachine::State::JOIN;
            } else if (is_message_type(server_msg, MessageBase::Type::ERR) ||
                       is_message_type(server_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            } else if (is_message_type(server_msg, MessageBase::Type::REPLY)) {
                current_state_ = StateMachine::State::END;
            } else if (is_message_type(client_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            }
            break;
        case StateMachine::State::JOIN:
            if (is_message_type(server_msg, MessageBase::Type::MSG)) {
                current_state_ = StateMachine::State::JOIN;
            } else if (is_message_type(server_msg, MessageBase::Type::ERR) ||
                       is_message_type(server_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            } else if (is_message_type(server_msg, MessageBase::Type::REPLY)) {
                current_state_ = StateMachine::State::OPEN;
            } else if (is_message_type(client_msg, MessageBase::Type::BYE)) {
                current_state_ = StateMachine::State::END;
            }
            break;
        case StateMachine::State::END:
            break;
    }

    printf_debug("New state=%d", static_cast<int>(current_state_));
    return current_state_;
}

// Checks if a message matches the specified type.
bool StateMachine::is_message_type(const std::shared_ptr<MessageBase>& msg, MessageBase::Type type) const {
    bool result = msg && msg->get_type() == type;
    printf_debug("Checking message type: expected=%d, actual=%d, result=%d",
                 static_cast<int>(type), msg ? static_cast<int>(msg->get_type()) : -1, result);
    return result;
}