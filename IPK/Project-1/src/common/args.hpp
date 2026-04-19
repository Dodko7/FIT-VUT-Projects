#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>
#include <cstdint>

/**
 * @struct ParsedArgs
 * @brief Stores parsed command-line arguments for the IPK25-CHAT client.
 */
struct ParsedArgs {
    std::string transport;         // Transport protocol ("tcp" or "udp")
    std::string server;            // Server IP address
    uint16_t port = 4567;          // Default server port
    uint16_t timeout = 250;        // Default UDP confirmation timeout in ms
    uint8_t retries = 3;           // Default maximum UDP retransmissions
};

/**
 * @brief Parses command-line arguments into a ParsedArgs structure.
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 * @return ParsedArgs structure with parsed arguments.
 * @throws std::invalid_argument on invalid arguments.
 */
ParsedArgs parse_arguments(int argc, char* argv[]);

/**
 * @brief Prints help information about command-line arguments.
 */
void print_help();

#endif // ARGS_HPP