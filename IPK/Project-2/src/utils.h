#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>
#include <sys/types.h>
#include <pcap/pcap.h>
#include <netinet/in.h>

// Structure to hold a list of IP addresses
typedef struct {
    struct sockaddr_storage *ip_addresses;  // Array of IP addresses (IPv4 or IPv6)
    int *families;                          // Array of address families (AF_INET for IPv4, AF_INET6 for IPv6)
    int count;                              // Number of stored addresses
} IpList;

// Function prototypes

/**
 * Resolves a hostname to one or more IP addresses.
 * @param hostname The hostname to resolve.
 * @param ip_list Pointer to an IpList structure to store the resolved addresses.
 * @return 0 on success, -1 on failure.
 */
int resolve_hostname(const char *hostname, IpList *ip_list);

/**
 * Frees memory allocated for an IpList structure.
 * @param ip_list Pointer to the IpList structure to free.
 */
void free_ip_list(IpList *ip_list);

/**
 * Prints the list of resolved IP addresses.
 * @param ip_list Pointer to the IpList structure.
 */
void print_ip_list(const IpList *ip_list);

/**
 * Prints all available network interfaces on the system.
 */
void print_interfaces(void);

/**
 * Verifies if a given network interface exists.
 * @param interface The name of the network interface to verify.
 * @return 1 if the interface exists, 0 otherwise.
 */
int verify_interface(const char *interface);

/**
 * Retrieves the source IP address of a given network interface.
 * @param interface The name of the network interface.
 * @param source_ip Buffer to store the retrieved IP address.
 * @param len The size of the buffer.
 * @param family Address family (AF_INET for IPv4, AF_INET6 for IPv6).
 * @return 0 on success, -1 on failure.
 */
int get_interface_ip(const char *interface, char *source_ip, size_t len, int family);

/**
 * Computes the Internet checksum (used in IP, ICMP, TCP, and UDP headers).
 * @param b Pointer to the data to calculate the checksum for.
 * @param len Length of the data.
 * @return Computed checksum.
 */
unsigned short checksum(void *b, int len);

#endif
