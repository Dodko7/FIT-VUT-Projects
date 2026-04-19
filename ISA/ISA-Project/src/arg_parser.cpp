/**
 * @file arg_parser.cpp
 * @brief Implementation of command-line argument parser
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-09-10
 */

#include "../include/arg_parser.h"
#include "../include/addr_resolver.h"
#include "../include/domain_utils.h"
#include "../include/logger.h"
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include <sstream>

// ============================================================================
// Forward Declarations
// ============================================================================

/**
 * @brief Parse port argument from command line
 * @param optarg Port string from command line
 * @param port Reference to store parsed port
 * @return true if valid, false otherwise
 */
static bool parse_port_argument(const char* optarg, uint16_t& port);

/**
 * @brief Parse timeout argument from command line
 * @param optarg Timeout string from command line
 * @param timeout Reference to store parsed timeout
 * @return true if valid, false otherwise
 */
static bool parse_timeout_argument(const char* optarg, int& timeout);

/**
 * @brief Validate parsed configuration
 * @param config Configuration to validate
 * @return true if valid, false otherwise
 */
static bool validate_parsed_config(const Config& config);

// ============================================================================
// Main Parsing Function
// ============================================================================

Config parse_arguments(int argc, char* argv[]) {
    // Initialize configuration structure
    Config config;

    // Flags to track required arguments
    bool server_set = false;
    bool filter_set = false;

    // Option character for getopt_long
    int opt;
    int option_index = 0;
    
    // Define long options
    static struct option long_options[] = {
        {"server",      required_argument, 0, 's'},
        {"port",        required_argument, 0, 'p'},
        {"filter",      required_argument, 0, 'f'},
        {"timeout",     required_argument, 0, 't'},
        {"verbose",     no_argument,       0, 'v'},
        {"help",        no_argument,       0, 'h'},
        {0, 0, 0, 0}  // Terminator
    };
    
    // Check for empty arguments
    if (argc < 2) {
        LOG_ERROR("No arguments provided");
        print_usage(argv[0]);
        return config;
    }
    
    // Initialize logger with default settings
    Logger::init(false);
    
    // Reset getopt state for safety
    optind = 1;
    
    // Parse all options using getopt_long()
    while ((opt = getopt_long(argc, argv, "s:p:f:t:vh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 's':  // Server address
                config.server = optarg;
                server_set = true;
                break;
            
            case 'p':  // Port number
                if (!parse_port_argument(optarg, config.port)) {
                    return config;  // Error already logged
                }
                break;
            
            case 'f':  // Filter file path
                config.filter_file = optarg;
                filter_set = true;
                break;
            
            case 't':  // Timeout in seconds
                if (!parse_timeout_argument(optarg, config.timeout)) {
                    return config;  // Error already logged
                }
                break;
            
            case 'v':  // Verbose mode
                config.verbose = true;
                Logger::init(true);  // Re-initialize with verbose enabled
                break;
            
            case 'h':  // Help message
                print_usage(argv[0]);
                std::exit(EXIT_SUCCESS);
                break;
            
            case '?':  // Unknown option or missing argument
                LOG_ERROR("Invalid command-line option");
                print_usage(argv[0]);
                return config;
            
            default:
                LOG_ERROR("Unexpected error during argument parsing");
                print_usage(argv[0]);
                return config;
        }
    }
    
    // Check for unexpected non-option arguments
    if (optind < argc) {
        // Build error message with all unexpected arguments
        std::ostringstream oss;
        oss << "Unexpected non-option arguments:";
        while (optind < argc) {
            oss << " '" << argv[optind++] << "'";
        }
        LOG_ERROR(oss.str());
        print_usage(argv[0]);
        return config;
    }
    
    // Check if server argument was provided
    if (!server_set) {
        LOG_ERROR("Required argument -s (server) not provided");
        print_usage(argv[0]);
        return config;
    }
    
    // Check if filter file argument was provided
    if (!filter_set) {
        LOG_ERROR("Required argument -f (filter file) not provided");
        print_usage(argv[0]);
        return config;
    }
    
    // Validate the parsed configuration
    if (!validate_parsed_config(config)) {
        return config;
    }
    
    // Resolve server address to sockaddr structure
    if (!resolve_server_address(config)) {
        LOG_ERROR("Failed to resolve server address");
        return config;
    }
    
    // All validation passed
    config.valid = true;
    
    // Log configuration summary using centralized logger
    Logger::log_config_summary(config);
    
    return config;
}

// ============================================================================
// Helper Functions
// ============================================================================

static bool parse_port_argument(const char* optarg, uint16_t& port) {
    // Pointer for strtol end position
    char* endptr;
    
    // Convert string to integer with error checking
    long port_long = std::strtol(optarg, &endptr, 10);
    
    // Check for non-numeric characters
    if (*endptr != '\0') {
        LOG_ERROR_F("Invalid port '%s' - must be numeric", optarg);
        return false;
    }
    
    // Validate port range
    if (port_long < DNS_PORT_MIN || port_long > DNS_PORT_MAX) {
        LOG_ERROR_F("Port %ld out of range (%u-%u)", 
                    port_long, DNS_PORT_MIN, DNS_PORT_MAX);
        return false;
    }
    
    // Assign safe value
    port = static_cast<uint16_t>(port_long);

    return true;
}

static bool parse_timeout_argument(const char* optarg, int& timeout) {
    // Pointer for strtol end position
    char* endptr;
    
    // Convert string to integer with error checking
    long timeout_long = std::strtol(optarg, &endptr, 10);
    
    // Check for non-numeric characters
    if (*endptr != '\0') {
        LOG_ERROR_F("Invalid timeout '%s' - must be numeric", optarg);
        return false;
    }
    
    // Validate timeout range (1-3600 seconds, i.e., 1 second to 1 hour)
    if (timeout_long < 1 || timeout_long > 3600) {
        LOG_ERROR_F("Timeout %ld out of range (1-3600 seconds)", timeout_long);
        return false;
    }
    
    // Assign safe value
    timeout = static_cast<int>(timeout_long);

    return true;
}

static bool validate_parsed_config(const Config& config) {
    // Server cannot be empty
    if (config.server.empty()) {
        LOG_ERROR("Server cannot be empty");
        return false;
    }
    
    // Validate server looks like IP or domain (soft check)
    if (!DomainUtils::is_valid_ipv4(config.server) && 
        !DomainUtils::is_valid_ipv6(config.server) &&
        !DomainUtils::validate_domain(config.server)) {
        LOG_WARNING_F("Server '%s' doesn't look like valid IP or domain - will attempt resolution", 
                    config.server.c_str());
    }
    
    // Filter file path cannot be empty
    if (config.filter_file.empty()) {
        LOG_ERROR("Filter file path cannot be empty");
        return false;
    }
    
    // Warn about privileged ports
    if (config.port < PRIVILEGED_PORT_MAX) {
        LOG_WARNING_F("Using privileged port %u - requires root privileges", 
                    config.port);
    }
    
    // Info about standard DNS port
    if (config.port == DNS_DEFAULT_PORT) {
        LOG_INFO("Using standard DNS port 53 - ensure no conflicts");
    }
    
    return true;
}

void print_usage(const char* program_name) {
    std::cerr << "\n";
    std::cerr << "Usage: " << program_name 
              << " -s server [-p port] -f filter_file [-t timeout] [-v] [-h]\n";
    std::cerr << "\n";
    std::cerr << "Required arguments:\n";
    std::cerr << "  -s, --server <server>       Upstream DNS resolver (IPv4/IPv6 or domain)\n";
    std::cerr << "  -f, --filter <filter_file>  Path to blocked domains file\n";
    std::cerr << "\n";
    std::cerr << "Optional arguments:\n";
    std::cerr << "  -p, --port <port>           Listening UDP port (default: 53, range: 1-65535)\n";
    std::cerr << "  -t, --timeout <seconds>     Query timeout in seconds (default: 10, range: 1-3600)\n";
    std::cerr << "  -v, --verbose               Enable verbose logging to stderr\n";
    std::cerr << "  -h, --help                  Display this help message and exit\n";
    std::cerr << "\n";
    std::cerr << "Examples:\n";
    std::cerr << "  " << program_name << " -s 8.8.8.8 -f blocked.txt\n";
    std::cerr << "  " << program_name << " --server dns.google --port 5300 --filter filter.txt --verbose\n";
    std::cerr << "  " << program_name << " -s 2001:4860:4860::8888 -f filter.txt --timeout 30\n";
    std::cerr << "  " << program_name << " --help\n";
    std::cerr << "\n";
}
