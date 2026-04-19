#include "output_handler.hpp"
#include "../common/debug.hpp"
#include <iostream>

/**
 * @brief Prints a message to stdout based on its type and the previous state.
 * @param msg The message to print.
 * @param prev_state The state before the message was received.
 */
void OutputHandler::print_message(const std::shared_ptr<MessageBase>& msg, StateMachine::State prev_state) {
    printf_debug("Printing message: type=%d, content=%s",
                 static_cast<int>(msg->get_type()), msg->get_content().c_str());

    switch (msg->get_type()) {
        case MessageBase::Type::REPLY:
            // Handle REPLY messages
            if (prev_state == StateMachine::State::AUTH || prev_state == StateMachine::State::JOIN) {
                std::cout << "Action " << (msg->is_success() ? "Success" : "Failure")
                          << ": " << msg->get_content() << "\n";
            } else {
                std::cout << "ERROR: Unexpected REPLY received\n";
            }
            break;
        case MessageBase::Type::MSG: {
            // Handle MSG messages
            std::string content = msg->get_content();
            size_t from_pos = content.find("FROM ") + 5;
            size_t is_pos = content.find(" IS ");
            if (from_pos != std::string::npos && is_pos != std::string::npos) {
                std::string sender = content.substr(from_pos, is_pos - from_pos);
                std::string message = content.substr(is_pos + 4);
                std::cout << sender << ": " << message << "\n";
            } else {
                std::cout << "ERROR: Invalid MSG format\n";
            }
            break;
        }
        case MessageBase::Type::ERR: {
            // Handle ERR messages
            std::string content = msg->get_content();
            size_t from_pos = content.find("FROM ") + 5;
            size_t is_pos = content.find(" IS ");
            if (from_pos != std::string::npos && is_pos != std::string::npos) {
                std::string sender = content.substr(from_pos, is_pos - from_pos);
                std::string message = content.substr(is_pos + 4);
                std::cout << "ERROR FROM " << sender << ": " << message << "\n";
            } else {
                std::cout << "ERROR: Invalid ERR format\n";
            }
            break;
        }
        default:
            break; // BYE, PING, CONFIRM do not produce output
    }
}

/**
 * @brief Prints an error message to stdout with consistent formatting.
 * @param error The error message.
 */
void OutputHandler::print_error(const std::string& error) {
    std::cout << "ERROR: " << error << "\n";
}