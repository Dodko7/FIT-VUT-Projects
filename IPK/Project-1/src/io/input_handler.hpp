#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "../messages/message_base.hpp"
#include "../state/state_machine.hpp"
#include "../common/message_validator.hpp"
#include <string>
#include <memory>

/**
 * @class InputHandler
 * @brief Processes user input and generates messages for the IPK25-CHAT client.
 */
class InputHandler {
public:
    /**
     * @brief Constructs an InputHandler for the specified transport protocol.
     * @param transport Transport protocol ("tcp" or "udp").
     */
    InputHandler(const std::string& transport);

    /**
     * @brief Processes user input and generates a message.
     * @param input The user input.
     * @param display_name Current display name (updated for /rename).
     * @param state Current state of the state machine.
     * @return Generated message or nullptr if no message is produced.
     */
    std::shared_ptr<MessageBase> process_input(const std::string& input,
                                              std::string& display_name,
                                              StateMachine::State state);

    // Prints help information about supported commands.
    void print_help() const;

private:
    std::string transport_; // Transport protocol ("tcp" or "udp")
};

#endif // INPUT_HANDLER_HPP