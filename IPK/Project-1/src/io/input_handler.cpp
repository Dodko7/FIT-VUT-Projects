#include "input_handler.hpp"
#include "../common/debug.hpp"
#include "../messages/tcp_message.hpp"
#include "../messages/udp_message.hpp"
#include <sstream>
#include <regex>
#include <iostream>

/**
 * @brief Constructs an InputHandler with the specified transport protocol.
 * @param transport The transport protocol ("tcp" or "udp").
 */
InputHandler::InputHandler(const std::string& transport) : transport_(transport) {
    printf_debug("Initializing InputHandler with transport: %s", transport.c_str());
}

/**
 * @brief Processes user input and generates a message based on the current state.
 * @param input The user input string.
 * @param display_name Current display name (updated for /rename).
 * @param state Current state of the state machine.
 * @return Generated message or nullptr if no message is produced.
 */
std::shared_ptr<MessageBase> InputHandler::process_input(const std::string& input,
                                                        std::string& display_name,
                                                        StateMachine::State state) {
    printf_debug("Processing input: %s", input.c_str());
    // Check for empty input
    if (input.empty()) {
        std::cout << "ERROR: Empty input\n";
        return nullptr;
    }

    // Parse input command
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    // Handle /auth command
    if (command == "/auth") {
        if (state != StateMachine::State::START && state != StateMachine::State::AUTH) {
            std::cout << "ERROR: Already authenticated\n";
            return nullptr;
        }
        std::string username, secret, displayname;
        iss >> username >> secret >> displayname;
        // Validate parameters
        if (!MessageValidator::validate_username_or_channel(username) ||
            !MessageValidator::validate_secret(secret) ||
            !MessageValidator::validate_display_name(displayname)) {
            std::cout << "ERROR: Invalid /auth parameters\n";
            return nullptr;
        }
        std::string content = username + " AS " + displayname + " USING " + secret;
        display_name = displayname;
        // Create message based on transport protocol
        if (transport_ == "tcp") {
            return std::make_shared<TcpMessage>("AUTH " + content);
        } else {
            // For UDP, use binary format for AUTH
            return std::make_shared<UdpMessage>(MessageBase::Type::AUTH, 0, content, false);
        }
    }
    // Handle /join command
    else if (command == "/join") {
        if (state != StateMachine::State::OPEN) {
            std::cout << "ERROR: Must be authenticated to use /join\n";
            return nullptr;
        }
        std::string channel;
        iss >> channel;
        if (!MessageValidator::validate_username_or_channel(channel)) {
            std::cout << "ERROR: Invalid /join channel\n";
            return nullptr;
        }
        if (display_name.empty()) {
            std::cout << "ERROR: No display name set\n";
            return nullptr;
        }
        std::string content = "JOIN " + channel + " AS " + display_name;
        if (transport_ == "tcp") {
            return std::make_shared<TcpMessage>(content);
        } else {
            // Use text format for JOIN
            return std::make_shared<UdpMessage>(MessageBase::Type::JOIN, 0, content, true);
        }
    } 
    // Handle /rename command
    else if (command == "/rename") {
        std::string new_displayname;
        iss >> new_displayname;
        if (!MessageValidator::validate_display_name(new_displayname)) {
            std::cout << "ERROR: Invalid /rename displayname\n";
            return nullptr;
        }
        display_name = new_displayname;
        std::cout << "Display name changed to: " << display_name << "\n";
        return nullptr;
    } 
    // Handle /help command
    else if (command == "/help") {
        print_help();
        return nullptr;
    } 
    // Handle /bye command
    else if (command == "/bye") {
        std::string content = "BYE FROM " + (display_name.empty() ? "unknown" : display_name);
        if (transport_ == "tcp") {
            return std::make_shared<TcpMessage>(content);
        } else {
            // Use text format for BYE
            return std::make_shared<UdpMessage>(MessageBase::Type::BYE, 0, content, true);
        }
    } 
    // Handle regular messages in OPEN state
    else if (state == StateMachine::State::OPEN) {
        if (display_name.empty()) {
            std::cout << "ERROR: No display name set\n";
            return nullptr;
        }
        if (!MessageValidator::validate_message_content(input)) {
            std::cout << "ERROR: Invalid message content\n";
            return nullptr;
        }
        std::string content = "MSG FROM " + display_name + " IS " + input;
        if (transport_ == "tcp") {
            return std::make_shared<TcpMessage>(content);
        } else {
            // Use text format for MSG
            return std::make_shared<UdpMessage>(MessageBase::Type::MSG, 0, content, true);
        }
    } 
    // Handle invalid commands
    else {
        std::cout << "ERROR: Unknown or invalid command\n";
        return nullptr;
    }
}

/**
 * @brief Prints help information for supported commands.
 */
void InputHandler::print_help() const {
    std::cout << "Supported commands:\n"
              << "  /auth {username} {secret} {displayname} - Authenticate\n"
              << "  /join {channelID} - Join a channel\n"
              << "  /rename {displayname} - Change display name\n"
              << "  /help - Display this help\n"
              << "  /bye - Terminate connection\n"
              << "  Any other input after authentication is sent as a message\n";
}