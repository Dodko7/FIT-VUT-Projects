#ifndef ARGS_H
#define ARGS_H

#define MAX_PORTS 65536  // Maximum valid port number

/**
 * Structure to hold parsed command-line arguments.
 */
typedef struct {
    char *interface;    // Name of the network interface to use
    char *target;       // Target hostname or IP address
    int *tcp_ports;     // Array of TCP ports to scan
    int *udp_ports;     // Array of UDP ports to scan
    int tcp_count;      // Number of TCP ports specified
    int udp_count;      // Number of UDP ports specified
    int timeout;        // Timeout for scanning (in milliseconds)
} Arguments;

/**
 * Parses a comma-separated port string and converts it into an array of port numbers.
 * @param port_str Input string containing port numbers (e.g., "22,80,443").
 * @param ports Pointer to an integer array where parsed ports will be stored.
 * @return Number of parsed ports on success, -1 on failure.
 */
int parse_ports(const char *port_str, int **ports);

/**
 * Parses command-line arguments and fills the Arguments structure.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @param args Pointer to the Arguments structure to populate.
 * @return 0 on success, -1 on failure.
 */
int parse_arguments(int argc, char *argv[], Arguments *args);

/**
 * Frees allocated memory inside the Arguments structure.
 * @param args Pointer to the Arguments structure.
 */
void free_arguments(Arguments *args);

#endif
