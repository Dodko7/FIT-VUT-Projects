#ifndef OUTPUT_HANDLER_HPP
#define OUTPUT_HANDLER_HPP

#include "../messages/message_base.hpp"
#include "../state/state_machine.hpp"

/**
 * @class OutputHandler
 * @brief Formats and prints messages to stdout for the IPK25-CHAT client.
 */
class OutputHandler {
public:
    /**
     * @brief Prints a message to stdout based on its type and previous state.
     * @param msg The message to print.
     * @param prev_state The state before the message was received.
     */
    void print_message(const std::shared_ptr<MessageBase>& msg, StateMachine::State prev_state);

    /**
     * @brief Prints an error message to stdout.
     * @param error The error message.
     */
    void print_error(const std::string& error);
};

#endif // OUTPUT_HANDLER_HPP