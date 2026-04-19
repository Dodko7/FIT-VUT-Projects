#include "args.hpp"
#include "debug.hpp"
#include <iostream>
#include <getopt.h>
#include <stdexcept>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

/**
 * @brief Prints help information about command-line arguments to stdout.
 */
void print_help() {
    std::cout << "Usage: ipk25chat-client -t {tcp|udp} -s <server> [-p <port>] [-d <timeout>] [-r <retries>] [-h]\n"
              << "  -t  Transport protocol (tcp or udp)\n"
              << "  -s  Server IP address or hostname\n"
              << "  -p  Server port (default: 4567)\n"
              << "  -d  UDP confirmation timeout in ms (default: 250)\n"
              << "  -r  Maximum number of UDP retransmissions (default: 3)\n"
              << "  -h  Print this help and exit\n";
}

/**
 * @brief Parses command-line arguments and returns a ParsedArgs structure.
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 * @return ParsedArgs structure with parsed arguments.
 * @throws std::invalid_argument on invalid arguments.
 */
ParsedArgs parse_arguments(int argc, char* argv[]) {
    ParsedArgs args;

    int opt;
    // Parse arguments using getopt
    while ((opt = getopt(argc, argv, "t:s:p:d:r:h")) != -1) {
        switch (opt) {
            case 't':
                args.transport = optarg;
                // Validate transport protocol
                if (args.transport != "tcp" && args.transport != "udp") {
                    throw std::invalid_argument("Invalid transport protocol. Use 'tcp' or 'udp'.");
                }
                break;
            case 's': {
                args.server = optarg;
                // Resolve server hostname or IP address
                struct addrinfo hints{}, *res;
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;

                if (getaddrinfo(args.server.c_str(), nullptr, &hints, &res) != 0) {
                    throw std::invalid_argument("Invalid server address or hostname: " + args.server);
                }

                // Convert resolved address to string
                char ip_str[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ip_str, sizeof(ip_str)) == nullptr) {
                    freeaddrinfo(res);
                    throw std::runtime_error("Failed to convert resolved address to string");
                }

                args.server = ip_str;
                freeaddrinfo(res);
                break;
            }
            case 'p':
                // Parse server port
                args.port = static_cast<uint16_t>(std::stoi(optarg));
                break;
            case 'd':
                // Parse UDP timeout
                args.timeout = static_cast<uint16_t>(std::stoi(optarg));
                break;
            case 'r':
                // Parse UDP retries
                args.retries = static_cast<uint8_t>(std::stoi(optarg));
                break;
            case 'h':
                // Print help and exit
                print_help();
                exit(0);
            default:
                throw std::invalid_argument("Unknown argument.");
        }
    }

    // Check for required arguments
    if (args.transport.empty() || args.server.empty()) {
        throw std::invalid_argument("Missing required arguments -t or -s.");
    }

    return args;
}