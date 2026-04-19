#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"
#include "utils.h"
#include "scanner.h"

// Main function: entry point of the port scanner
int main(int argc, char *argv[]) {
    Arguments args = {0};  // Initialize arguments structure

    // Parse command-line arguments
    if (parse_arguments(argc, argv, &args) < 0) {
        free_arguments(&args);
        return EXIT_FAILURE;  // Exit on parsing error
    }

    // Verify the specified network interface
    if (args.interface && verify_interface(args.interface) < 0) {
        fprintf(stderr, "Invalid or inactive interface: %s\n", args.interface);
        free_arguments(&args);
        return EXIT_FAILURE;
    }

    IpList ip_list;  // Initialize IP list structure

    // Resolve target hostname to IP addresses
    if (args.target && resolve_hostname(args.target, &ip_list) < 0) {
        fprintf(stderr, "Failed to resolve hostname: %s\n", args.target);
        free_arguments(&args);
        return EXIT_FAILURE;
    }

    // Check if any IP addresses were resolved
    if (ip_list.count == 0 && args.target) {
        fprintf(stderr, "No IP addresses resolved for: %s\n", args.target);
        free_arguments(&args);
        return EXIT_FAILURE;
    }

    // Perform the port scanning operation with the parsed timeout
    if (args.target && scan(&args, &ip_list) < 0) {
        fprintf(stderr, "Scanning failed\n");
        free_ip_list(&ip_list);
        free_arguments(&args);
        return EXIT_FAILURE;
    }

    // Clean up allocated memory
    free_ip_list(&ip_list);
    free_arguments(&args);
    return EXIT_SUCCESS; 
}