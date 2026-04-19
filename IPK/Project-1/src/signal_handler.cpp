#include "signal_handler.hpp"
#include "client_application.hpp"
#include "common/debug.hpp"

// Static member initialization
ClientApplication* SignalHandler::app_ = nullptr;

/**
 * Constructs the signal handler and registers SIGINT handler.
 */
SignalHandler::SignalHandler(ClientApplication& app) {
    app_ = &app;
    termination_requested_ = 0;
    printf_debug("Registering SIGINT handler");
    signal(SIGINT, handle_signal);
}

/**
 * Static handler for SIGINT signal.
 * Sets the termination flag.
 */
void SignalHandler::handle_signal(int) {
    printf_debug("Caught SIGINT");
    if (app_) {
        app_->get_signal_handler().termination_requested_ = 1;
    }
}