#include "common/args.hpp"
#include "common/debug.hpp"
#include "client_application.hpp"
#include <iostream>

/**
 * @brief Entry point of the IPK25-CHAT client.
 * Parses command-line arguments and starts the client application.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code (0 for success, 1 for error).
 */
int main(int argc, char* argv[]) {
    printf_debug("Starting client");

    // Parse command-line arguments
    ParsedArgs args;
    try {
        args = parse_arguments(argc, argv);
    } catch (const std::exception& e) {
        printf_debug("ERROR: Failed to parse arguments: %s", e.what());
        std::cout << "ERROR: " << e.what() << "\n";
        print_help();
        return 1;
    }

    // Initialize and run the client application
    try {
        ClientApplication app(args);
        return app.run();
    } catch (const std::exception& e) {
        printf_debug("ERROR: Client application failed: %s", e.what());
        std::cout << "ERROR: Client application failed: " << e.what() << "\n";
        return 1;
    }
}