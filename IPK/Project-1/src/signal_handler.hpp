#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include "network/client.hpp"
#include "state/state_machine.hpp"
#include <csignal>

// Forward declaration of ClientApplication to avoid circular dependency
class ClientApplication;

/**
 * @class SignalHandler
 * @brief Handles SIGINT (Ctrl+C) and EOF (Ctrl+D) signals for the IPK25-CHAT client.
 */
class SignalHandler {
public:
    /**
     * @brief Constructs the signal handler with a reference to the application.
     * @param app Reference to the ClientApplication instance.
     */
    SignalHandler(ClientApplication& app);

    /**
     * @brief Checks if termination was requested (via SIGINT or EOF).
     * @return True if termination was requested, false otherwise.
     */
    bool is_termination_requested() const { return termination_requested_; }

    /**
     * @brief Requests termination (e.g., on EOF).
     */
    void request_termination() { termination_requested_ = true; }

private:
    /**
     * @brief Static signal handler function for SIGINT.
     * @param sig Signal number.
     */
    static void handle_signal(int sig);

    static ClientApplication* app_; // Reference to the application
    volatile sig_atomic_t termination_requested_ = 0; // Termination flag
};

#endif // SIGNAL_HANDLER_HPP