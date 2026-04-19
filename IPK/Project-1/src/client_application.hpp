#ifndef CLIENT_APPLICATION_HPP
#define CLIENT_APPLICATION_HPP

#include "common/args.hpp"
#include "network/client.hpp"
#include "io/input_handler.hpp"
#include "io/output_handler.hpp"
#include "state/state_machine.hpp"
#include "signal_handler.hpp"
#include <memory>

/**
 * @class ClientApplication
 * @brief Coordinates the main loop and components of the IPK25-CHAT client.
 *        Manages Client, StateMachine, InputHandler, OutputHandler, and SignalHandler.
 */
class ClientApplication {
public:
    /**
     * @brief Constructs the application with parsed command-line arguments.
     * @param args Parsed command-line arguments.
     */
    ClientApplication(const ParsedArgs& args);

    /**
     * @brief Runs the main loop of the application.
     * @return Exit code (0 for success, 1 for error).
     */
    int run();

    /**
     * @brief Provides access to the signal handler for SIGINT handling.
     * @return Reference to the signal handler.
     */
    SignalHandler& get_signal_handler() { return signal_handler_; }

private:
    /**
     * @brief Initializes the client based on the transport protocol.
     * @throws std::exception on initialization failure.
     */
    void initialize_client();

    /**
     * @brief Handles user input from stdin.
     * @param fds Poll file descriptors.
     * @return True if input was processed successfully, false on termination.
     */
    bool handle_stdin(struct pollfd* fds);

    /**
     * @brief Handles messages received from the server.
     * @return True if messages were processed successfully, false on termination.
     */
    bool handle_server_input();

    /**
     * @brief Checks for timeout on AUTH/JOIN requests.
     * @return True if no timeout occurred, false if timeout handling terminated the program.
     */
    bool check_request_timeout();

    std::unique_ptr<Client> client_;       // Network client (TCP or UDP)
    StateMachine fsm_;                     // Finite state machine for protocol states
    InputHandler input_handler_;           // Handles user input
    OutputHandler output_handler_;         // Handles output to stdout
    SignalHandler signal_handler_;         // Handles SIGINT and EOF
    ParsedArgs args_;                      // Command-line arguments
    std::string display_name_;             // User's display name
    std::shared_ptr<MessageBase> last_client_msg_; // Last message sent by client
    time_t last_request_time_ = 0;         // Timestamp of last AUTH/JOIN request
};

#endif // CLIENT_APPLICATION_HPP