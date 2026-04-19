#include "client_application.hpp"
#include "common/debug.hpp"
#include "messages/tcp_message.hpp"
#include "messages/udp_message.hpp"
#include "network/tcp_client.hpp"
#include "network/udp_client.hpp"
#include <poll.h>
#include <cstring>
#include <iostream>

/**
 * Constructs the application with parsed command-line arguments.
 */
ClientApplication::ClientApplication(const ParsedArgs& args)
    : input_handler_(args.transport), signal_handler_(*this), args_(args) {
    printf_debug("Initializing ClientApplication");
    initialize_client();
}

/**
 * Initializes the client based on the transport protocol.
 */
void ClientApplication::initialize_client() {
    printf_debug("Initializing client: transport=%s", args_.transport.c_str());
    if (args_.transport == "tcp") {
        client_ = std::make_unique<TcpClient>(args_.server, args_.port);
    } else {
        client_ = std::make_unique<UdpClient>(args_.server, args_.port, args_.timeout, args_.retries);
    }
}

/**
 * Runs the main loop of the application.
 * Polls stdin and server socket, processes input/output, and handles signals.
 */
int ClientApplication::run() {
    printf_debug("Starting main loop");

    // Set up polling for stdin and server socket
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = client_->get_socket_fd();
    fds[1].events = POLLIN;

    // Main loop: continue until FSM reaches END state
    while (fsm_.get_current_state() != StateMachine::State::END) {
        // Poll stdin and server socket with 5-second timeout
        printf_debug("Polling for events");
        int poll_result = poll(fds, 2, 5000);
        if (poll_result < 0) {
            if (errno == EINTR) continue; // Ignore interrupted system calls
            output_handler_.print_error("Poll failed: " + std::string(strerror(errno)));
            client_->terminate();
            return 1;
        }

        // Check for timeout on AUTH/JOIN requests
        if (!check_request_timeout()) {
            return 1;
        }

        // Handle SIGINT or EOF if signaled
        if (signal_handler_.is_termination_requested() &&
            fsm_.get_current_state() != StateMachine::State::AUTH &&
            fsm_.get_current_state() != StateMachine::State::JOIN) {
            printf_debug("Processing termination request (SIGINT or EOF)");
            std::shared_ptr<MessageBase> bye_msg;
            if (args_.transport == "tcp") {
                bye_msg = std::make_shared<TcpMessage>("BYE FROM " + (display_name_.empty() ? "unknown" : display_name_));
            } else {
                bye_msg = std::make_shared<UdpMessage>(MessageBase::Type::BYE, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_));
            }
            try {
                client_->send_message(bye_msg);
                fsm_.transition(nullptr, bye_msg);
            } catch (const std::exception& e) {
                printf_debug("ERROR: Failed to send BYE: %s", e.what());
                output_handler_.print_error("Failed to send BYE: " + std::string(e.what()));
            }
            client_->terminate();
            return 0;
        }

        // Handle stdin input (user commands or messages)
        if (fds[0].revents & (POLLIN | POLLHUP | POLLERR)) {
            if (!handle_stdin(fds)) {
                return 0;
            }
        }

        // Handle server messages
        if (fds[1].revents & POLLIN) {
            if (!handle_server_input()) {
                return 0; // BYE received or termination requested
            }
        }
    }

    // Final cleanup before exit
    printf_debug("Client terminated");
    client_->terminate();
    return 0;
}

/**
 * Handles user input from stdin, including commands and messages.
 * Returns false if the program should terminate (e.g., EOF or BYE).
 */
bool ClientApplication::handle_stdin(struct pollfd* fds) {
    printf_debug("Handling stdin input");
    if (fds[0].revents & POLLIN) {
        std::string input;
        std::getline(std::cin, input);
        // Handle EOF or stdin closure (Ctrl+D)
        if (std::cin.eof() || (fds[0].revents & (POLLHUP | POLLERR))) {
            printf_debug("Received EOF or stdin closure");
            signal_handler_.request_termination();
            return true; // Termination will be handled in the main loop
        }

        // Process user input
        auto msg = input_handler_.process_input(input, display_name_, fsm_.get_current_state());
        if (msg) {
            try {
                client_->send_message(msg);
                last_client_msg_ = msg;
                if (msg->get_type() == MessageBase::Type::AUTH || msg->get_type() == MessageBase::Type::JOIN) {
                    last_request_time_ = time(nullptr);
                }
                fsm_.transition(nullptr, msg);
            } catch (const std::exception& e) {
                output_handler_.print_error("Failed to send message: " + std::string(e.what()));
                client_->terminate();
                return false;
            }
        }
    } else if (fds[0].revents & (POLLHUP | POLLERR)) {
        printf_debug("Received stdin closure (POLLHUP or POLLERR)");
        signal_handler_.request_termination();
        return true; // Termination will be handled in the main loop
    }
    return true;
}

/**
 * Handles messages received from the server.
 * Returns false if the program should terminate (e.g., BYE received).
 */
bool ClientApplication::handle_server_input() {
    printf_debug("Handling server input");
    try {
        auto messages = client_->receive_message();
        if (messages.empty()) {
            printf_debug("No valid messages received");
            return true; // Continue processing
        }
        for (const auto& msg : messages) {
            if (!msg) continue;
            StateMachine::State prev_state = fsm_.get_current_state();
            fsm_.transition(msg, nullptr);
            output_handler_.print_message(msg, prev_state);

            // Handle unexpected REPLY in OPEN state
            if (msg->get_type() == MessageBase::Type::REPLY &&
                prev_state == StateMachine::State::OPEN) {
                output_handler_.print_error("Unexpected REPLY received");
                std::shared_ptr<MessageBase> err_msg;
                if (args_.transport == "tcp") {
                    err_msg = std::make_shared<TcpMessage>("ERR FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS Unexpected REPLY");
                } else {
                    err_msg = std::make_shared<UdpMessage>(MessageBase::Type::ERR, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS Unexpected REPLY");
                }
                try {
                    client_->send_message(err_msg);
                } catch (const std::exception& e) {
                    printf_debug("ERROR: Failed to send ERR: %s", e.what());
                    output_handler_.print_error("Failed to send ERR: " + std::string(e.what()));
                }
                client_->terminate();
                return false;
            }
            // Handle unexpected MSG in AUTH state
            else if (msg->get_type() == MessageBase::Type::MSG &&
                     prev_state == StateMachine::State::AUTH) {
                output_handler_.print_error("Unexpected MSG received");
                std::shared_ptr<MessageBase> err_msg;
                if (args_.transport == "tcp") {
                    err_msg = std::make_shared<TcpMessage>("ERR FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS Unexpected MSG");
                } else {
                    err_msg = std::make_shared<UdpMessage>(MessageBase::Type::ERR, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS Unexpected MSG");
                }
                try {
                    client_->send_message(err_msg);
                } catch (const std::exception& e) {
                    printf_debug("ERROR: Failed to send ERR: %s", e.what());
                    output_handler_.print_error("Failed to send ERR: " + std::string(e.what()));
                }
                client_->terminate();
                return false;
            }
            // Handle BYE message
            else if (msg->get_type() == MessageBase::Type::BYE) {
                std::shared_ptr<MessageBase> bye_msg;
                if (args_.transport == "tcp") {
                    bye_msg = std::make_shared<TcpMessage>("BYE FROM " + (display_name_.empty() ? "unknown" : display_name_));
                } else {
                    bye_msg = std::make_shared<UdpMessage>(MessageBase::Type::BYE, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_));
                }
                try {
                    client_->send_message(bye_msg);
                    fsm_.transition(nullptr, bye_msg);
                } catch (const std::exception& e) {
                    printf_debug("ERROR: Failed to send BYE: %s", e.what());
                    output_handler_.print_error("Failed to send BYE: " + std::string(e.what()));
                }
                client_->terminate();
                return false;
            }
            // Handle ERR message
            else if (msg->get_type() == MessageBase::Type::ERR) {
                client_->terminate();
                return false;
            }

            // Handle delayed termination request
            if (signal_handler_.is_termination_requested() &&
                fsm_.get_current_state() != StateMachine::State::AUTH &&
                fsm_.get_current_state() != StateMachine::State::JOIN) {
                printf_debug("Processing delayed termination request");
                std::shared_ptr<MessageBase> bye_msg;
                if (args_.transport == "tcp") {
                    bye_msg = std::make_shared<TcpMessage>("BYE FROM " + (display_name_.empty() ? "unknown" : display_name_));
                } else {
                    bye_msg = std::make_shared<UdpMessage>(MessageBase::Type::BYE, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_));
                }
                try {
                    client_->send_message(bye_msg);
                    fsm_.transition(nullptr, bye_msg);
                } catch (const std::exception& e) {
                    printf_debug("ERROR: Failed to send BYE: %s", e.what());
                    output_handler_.print_error("Failed to send BYE: " + std::string(e.what()));
                }
                client_->terminate();
                return false;
            }
        }
    } catch (const std::exception& e) {
        output_handler_.print_error("Malformed message received: " + std::string(e.what()));
        std::shared_ptr<MessageBase> err_msg;
        if (args_.transport == "tcp") {
            err_msg = std::make_shared<TcpMessage>("ERR FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS Malformed message");
        } else {
            err_msg = std::make_shared<UdpMessage>(MessageBase::Type::ERR, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS Malformed message");
        }
        try {
            client_->send_message(err_msg);
        } catch (const std::exception& e) {
            printf_debug("ERROR: Failed to send ERR: %s", e.what());
            output_handler_.print_error("Failed to send ERR: " + std::string(e.what()));
        }
        client_->terminate();
        return false;
    }
    return true;
}

/**
 * Checks for timeout on AUTH/JOIN requests (5 seconds).
 * Sends ERR and terminates if timeout occurs.
 */
bool ClientApplication::check_request_timeout() {
    StateMachine::State current_state = fsm_.get_current_state();
    if (last_request_time_ != 0 &&
        (current_state == StateMachine::State::AUTH || current_state == StateMachine::State::JOIN) &&
        (time(nullptr) - last_request_time_ >= 5)) {
        output_handler_.print_error("No reply received within 5 seconds");
        std::shared_ptr<MessageBase> err_msg;
        if (args_.transport == "tcp") {
            err_msg = std::make_shared<TcpMessage>("ERR FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS No reply received");
        } else {
            err_msg = std::make_shared<UdpMessage>(MessageBase::Type::ERR, 0, "FROM " + (display_name_.empty() ? "unknown" : display_name_) + " IS No reply received");
        }
        try {
            client_->send_message(err_msg);
        } catch (const std::exception& e) {
            printf_debug("ERROR: Failed to send ERR: %s", e.what());
            output_handler_.print_error("Failed to send ERR: " + std::string(e.what()));
        }
        client_->terminate();
        return false;
    }
    return true;
}