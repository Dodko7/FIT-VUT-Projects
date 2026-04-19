#define _POSIX_C_SOURCE 200809L
#include <stdint.h>       // For potential future use (e.g., uint16_t for ports)
#include <stdio.h>        // For printf, fprintf, sscanf
#include <stdlib.h>       // For malloc, free, atoi, exit
#include <string.h>       // For strdup, strcmp, strchr, strtok
#include <getopt.h>       // For getopt_long and struct option
#include <stdbool.h>      // For bool type and true/false
#include "args.h"         // For Arguments structure and function prototypes
#include "utils.h"        // For print_interfaces function

#define MAX_PORTS 65536  // Maximum valid port number (2^16, range 0-65535)

// Parses a comma-separated list of ports or port ranges into a dynamically allocated array
// Input: port_str (e.g., "22,80,100-200"), pointer to ports array
// Output: number of ports parsed, or -1 on error; ports array is allocated and filled
int parse_ports(const char *port_str, int **ports) {
    int count = 0;                      // Tracks the number of ports parsed
    int *temp_ports = malloc(MAX_PORTS * sizeof(int));  // Temporary array to hold ports (max size)
    if (!temp_ports) {
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }

    char *str = strdup(port_str);       // Creates a mutable copy of the input string for tokenization
    if (!str) {
        free(temp_ports);
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }

    char *token = strtok(str, ",");     // Splits the string by commas to get individual ports/ranges
    while (token) {
        if (strchr(token, '-')) {       // If token contains '-', it's a range (e.g., "100-200")
            int start, end;
            if (sscanf(token, "%d-%d", &start, &end) != 2 ||  // Parses range into start and end
                start < 0 || end > 65535 || start > end) {    // Validates range bounds
                fprintf(stderr, "Invalid port range: %s\n", token);
                free(temp_ports);
                free(str);
                return -1;
            }
            for (int i = start; i <= end; i++) {  // Adds each port in the range to the array
                temp_ports[count++] = i;
            }
        } else {                        // Single port (e.g., "22")
            int port = atoi(token);     // Converts string to integer
            if (port < 0 || port > 65535) {  // Validates port number
                fprintf(stderr, "Invalid port: %s\n", token);
                free(temp_ports);
                free(str);
                return -1;
            }
            temp_ports[count++] = port;  // Adds port to the array
        }
        token = strtok(NULL, ",");       // Moves to the next token
    }

    if (!(*ports = realloc(temp_ports, count * sizeof(int)))) {  // Resizes array to exact size needed
        free(temp_ports);
        free(str);
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }
    free(str);                           // Frees the duplicated string
    return count;                        // Returns the number of ports parsed
}

// Parses command-line arguments into the Arguments structure
// Input: argc and argv from main, pointer to Arguments structure
// Output: 0 on success, -1 on error; args is filled with parsed values
int parse_arguments(int argc, char *argv[], Arguments *args) {
    int opt;                             // Holds the option character from getopt_long
    bool interface_flag = false;         // Tracks if -i/--interface is present

    static struct option long_options[] = {  // Defines long options for getopt_long
        {"interface", required_argument, 0, 'i'},  // -i or --interface with required value
        {"pt", required_argument, 0, 't'},         // --pt for TCP ports
        {"pu", required_argument, 0, 'u'},         // --pu for UDP ports
        {"wait", required_argument, 0, 'w'},       // -w or --wait for timeout
        {"help", no_argument, 0, 'h'},             // -h or --help for usage info
        {0, 0, 0, 0}                               // Terminator for the array
    };

    // Initializes Arguments structure with default values
    args->interface = NULL;              // No default interface
    args->tcp_ports = NULL;              // No TCP ports initially
    args->tcp_count = 0;                 // Count of TCP ports
    args->udp_ports = NULL;              // No UDP ports initially
    args->udp_count = 0;                 // Count of UDP ports
    args->timeout = 5000;                // Default timeout of 5000 ms as per specification
    args->target = NULL;                 // No default target

    // If no arguments are provided (just the program name), print interfaces and exit
    if (argc == 1) {
        print_interfaces();              // Displays available network interfaces
        exit(EXIT_SUCCESS);
    }

    // Checks if -i/--interface is used without a value and no other arguments
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--interface")) {
            if (i == argc - 1 || (i + 1 < argc && argv[i + 1][0] == '-')) {
                print_interfaces();      // Prints interfaces if -i/--interface is alone or last
                exit(EXIT_SUCCESS);
            } else {
                interface_flag = true;   // Indicates -i/--interface was used with a value
            }
        }
    }

    // Parses options using getopt_long
    while ((opt = getopt_long(argc, argv, "i:t:u:w:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'i':                    // Handles -i/--interface option
                if (!optarg) {           // Ensures a value is provided
                    fprintf(stderr, "Error: -i/--interface requires a value.\n");
                    return -1;
                }
                args->interface = strdup(optarg);  // Stores the interface name
                if (!args->interface) {
                    fprintf(stderr, "Memory allocation error\n");
                    return -1;
                }
                break;
            case 't':                    // Handles -t/--pt (TCP ports)
                args->tcp_count = parse_ports(optarg, &args->tcp_ports);
                if (args->tcp_count < 0) return -1;  // Error if parsing fails
                break;
            case 'u':                    // Handles -u/--pu (UDP ports)
                args->udp_count = parse_ports(optarg, &args->udp_ports);
                if (args->udp_count < 0) return -1;  // Error if parsing fails
                break;
            case 'w':                    // Handles -w/--wait (timeout)
                args->timeout = atoi(optarg);
                if (args->timeout <= 0) {  // Validates timeout is positive
                    fprintf(stderr, "Invalid timeout: %s\n", optarg);
                    return -1;
                }
                break;
            case 'h':                    // Handles -h/--help
                printf("Usage: %s [-i interface] [-t/--pt ports] [-u/--pu ports] [-w timeout] [hostname | ip]\n", argv[0]);
                exit(EXIT_SUCCESS);
            default:                     // Handles unknown options
                fprintf(stderr, "Invalid argument. Use --help for help.\n");
                return -1;
        }
    }

    // Ensures -i with a value is not misinterpreted as a standalone flag
    if (interface_flag && !args->interface) {
        fprintf(stderr, "Error: -i/--interface without a value is only allowed without additional arguments.\n");
        return -1;
    }

    // Stores the target (hostname or IP) if provided after options
    if (optind < argc) {
        args->target = strdup(argv[optind]);
        if (!args->target) {
            fprintf(stderr, "Memory allocation error\n");
            return -1;
        }
    } else if (!args->interface) {  // Requires target if no standalone -i
        fprintf(stderr, "Missing hostname or IP address.\n");
        return -1;
    }

    // Ensures at least one port type is specified when a target is provided
    if (args->target && args->tcp_count == 0 && args->udp_count == 0) {
        fprintf(stderr, "At least one of -t or -u must be specified with a target.\n");
        return -1;
    }

    return 0;  // Successfully parsed all arguments
}

// Frees dynamically allocated memory in the Arguments structure
// Input: Pointer to Arguments structure
// Output: None (memory is freed)
void free_arguments(Arguments *args) {
    if (args == NULL) return;           // Nothing to free if args is NULL

    free(args->interface);              // Frees the interface string if allocated
    free(args->target);                 // Frees the target string if allocated
    if (args->tcp_ports) {              // Frees TCP ports array if allocated
        free(args->tcp_ports);
    }
    if (args->udp_ports) {              // Frees UDP ports array if allocated
        free(args->udp_ports);
    }
}