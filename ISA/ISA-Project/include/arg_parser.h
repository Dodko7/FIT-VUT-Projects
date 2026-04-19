/**
 * @file arg_parser.h
 * @brief Command-line argument parser for DNS filter proxy
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-09-10
 * 
 * Responsibilities:
 * - Parse command-line arguments (-s server, -p port, -f filter_file, -t timeout, -v verbose)
 * - Validate argument values and required parameters
 * - Resolve upstream DNS server address using getaddrinfo()
 * - Provide configuration structure containing parsed settings and resolved addresses
 * - Handle usage information display for invalid arguments
 */

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "common.h"
#include <string>
#include <cstring>
#include <netinet/in.h>

/**
 * @brief Configuration structure containing all parsed and validated settings
 * 
 * Stores command-line arguments, validation status, and resolved network addresses.
 * All fields initialized to safe defaults in constructor.
 */
struct Config {
    // Command-line arguments
    std::string server;         // -s: upstream DNS server (IP or domain)
    uint16_t port;              // -p: listening port (default: 53)
    std::string filter_file;    // -f: path to filter file
    bool verbose;               // -v: verbose logging flag
    int timeout;                // -t: query timeout in seconds (default: 10)
    
    // Parse status
    bool valid;                 // true if all parsing/validation succeeded
    
    // Resolved server address (populated after getaddrinfo)
    sockaddr_storage server_addr;    // Resolved address structure
    socklen_t server_addr_len;       // Length of server_addr
    
    // Constructor with sensible defaults
    Config() 
        : port(DNS_DEFAULT_PORT)
        , verbose(false)
        , timeout(ID_MAPPING_TIMEOUT_SEC)
        , valid(false)
        , server_addr_len(0) 
    {
        // Zero-initialize sockaddr_storage
        std::memset(&server_addr, 0, sizeof(server_addr));
    }
};

/**
 * @brief Parse and validate all command-line arguments
 * 
 * Processing steps:
 * 1. Parse options using getopt_long() (supports -s/--server, -p/--port, etc.)
 * 2. Validate required arguments are present (-s and -f)
 * 3. Validate numeric ranges (port: 1-65535, timeout: 1-3600)
 * 4. Validate server address format (IPv4, IPv6, or domain name)
 * 5. Resolve server hostname to sockaddr structure using getaddrinfo()
 * 6. Set config.valid = true only if all validation passes
 * 
 * @param argc Argument count from main()
 * @param argv Argument vector from main()
 * @return Config structure (always check config.valid before use)
 */
Config parse_arguments(int argc, char* argv[]);

/**
 * @brief Display usage information and examples to stderr
 * 
 * Shows:
 * - Command-line syntax with required and optional arguments
 * - Description of each flag (short and long forms)
 * - Default values and valid ranges
 * - Usage examples for common scenarios
 * 
 * @param program_name Program name from argv[0]
 */
void print_usage(const char* program_name);

#endif // ARG_PARSER_H
