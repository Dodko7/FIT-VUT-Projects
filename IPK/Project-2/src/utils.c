#define _POSIX_C_SOURCE 200809L  // Ensures POSIX compatibility (e.g., for getaddrinfo, pcap)
#include <stdint.h>              // For uint32_t, uint16_t in checksum (optional)
#include <sys/types.h>           // For system types (not used directly)
#include <stdio.h>               // For printf, fprintf
#include <stdlib.h>              // For malloc, free
#include <string.h>              // For strcmp, strlen, memcpy
#include <unistd.h>              // For close (not used directly)
#include <sys/socket.h>          // For struct sockaddr, AF_INET, AF_INET6
#include <sys/ioctl.h>           // For ioctl (not used directly)
#include <net/if.h>              // For struct ifreq (not used directly)
#include <netdb.h>               // For getaddrinfo, gai_strerror
#include <arpa/inet.h>           // For inet_ntop
#include <pcap/pcap.h>           // For pcap_if_t, pcap_findalldevs
#include <ifaddrs.h>             // For getifaddrs, struct ifaddrs
#include "utils.h"               // For IpList, function prototypes

// Verifies if a network interface exists and is active
// Input: interface (interface name)
// Output: 0 (active), 1 (exists but no address), -1 (not found), -2 (invalid input), -3 (pcap error)
int verify_interface(const char *interface) {
    pcap_if_t *alldevs, *dev;           // Structures for pcap device list
    char errbuf[PCAP_ERRBUF_SIZE];      // Buffer for pcap errors

    if (!interface || strlen(interface) == 0) return -2;  // Checks for invalid input

    if (pcap_findalldevs(&alldevs, errbuf) == -1) return -3;  // Retrieves all devices, checks for errors

    int result = -1;                    // Default result: interface not found
    for (dev = alldevs; dev != NULL; dev = dev->next) {  // Iterates over all devices
        if (strcmp(dev->name, interface) == 0) {         // Matches interface name
            result = (dev->addresses != NULL) ? 0 : 1;   // 0 if active, 1 if exists but no addresses
            break;
        }
    }

    pcap_freealldevs(alldevs);          // Frees device list
    return result;                      // Returns result
}

// Resolves a hostname to a list of unique IP addresses
// Input: hostname (domain name), ip_list (pointer to IpList structure)
// Output: 0 on success, -1 on failure; fills ip_list with resolved IPs
int resolve_hostname(const char *hostname, IpList *ip_list) {
    struct addrinfo hints, *result, *p;  // Structures for address resolution
    int status;

    memset(&hints, 0, sizeof(hints));   // Initializes hints structure
    hints.ai_family = AF_UNSPEC;        // Supports both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP socket type (for consistency)
    hints.ai_protocol = IPPROTO_TCP;    // TCP protocol

    status = getaddrinfo(hostname, NULL, &hints, &result);  // Resolves hostname
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    int count = 0;                      // Counts total resolved addresses
    for (p = result; p != NULL; p = p->ai_next) {
        count++;
    }
    ip_list->ip_addresses = malloc(count * sizeof(struct sockaddr_storage));  // Allocates memory for addresses
    ip_list->families = malloc(count * sizeof(int));                         // Allocates memory for families
    ip_list->count = 0;                                                      // Initializes count

    int i = 0;                          // Index for unique addresses
    for (p = result; p != NULL; p = p->ai_next) {  // Iterates over resolved addresses
        char ip_str[INET6_ADDRSTRLEN];
        void *addr = (p->ai_family == AF_INET) ?   // Extracts IP address
                     (void *)&((struct sockaddr_in *)p->ai_addr)->sin_addr :
                     (void *)&((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
        inet_ntop(p->ai_family, addr, ip_str, sizeof(ip_str));  // Converts to string

        int duplicate = 0;                 // Flag for duplicate check
        for (int j = 0; j < i; j++) {      // Checks for duplicates
            char existing_ip[INET6_ADDRSTRLEN];
            void *existing_addr = (ip_list->families[j] == AF_INET) ?
                                 (void *)&((struct sockaddr_in *)&ip_list->ip_addresses[j])->sin_addr :
                                 (void *)&((struct sockaddr_in6 *)&ip_list->ip_addresses[j])->sin6_addr;
            inet_ntop(ip_list->families[j], existing_addr, existing_ip, sizeof(existing_ip));
            if (strcmp(ip_str, existing_ip) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {                  // Adds unique address
            ip_list->families[i] = p->ai_family;  // Stores address family
            memcpy(&ip_list->ip_addresses[i], p->ai_addr, p->ai_addrlen);  // Copies address
            i++;
        }
    }
    ip_list->count = i;                    // Updates final count

    freeaddrinfo(result);                  // Frees resolved address list
    return 0;                              // Success
}

// Frees memory allocated for the IP list
// Input: ip_list (pointer to IpList structure)
// Output: None; resets ip_list fields to safe values
void free_ip_list(IpList *ip_list) {
    free(ip_list->ip_addresses);           // Frees address array
    free(ip_list->families);               // Frees family array
    ip_list->ip_addresses = NULL;          // Resets pointer
    ip_list->families = NULL;              // Resets pointer
    ip_list->count = 0;                    // Resets count
}

// Prints all IP addresses in the list
// Input: ip_list (pointer to IpList structure)
// Output: None; prints IPs to stdout
void print_ip_list(const IpList *ip_list) {
    for (int i = 0; i < ip_list->count; i++) {  // Iterates over IP list
        char ip_str[INET6_ADDRSTRLEN];
        void *addr = (ip_list->families[i] == AF_INET) ?  // Extracts IP address
                     (void *)&((struct sockaddr_in *)&ip_list->ip_addresses[i])->sin_addr :
                     (void *)&((struct sockaddr_in6 *)&ip_list->ip_addresses[i])->sin6_addr;
        inet_ntop(ip_list->families[i], addr, ip_str, sizeof(ip_str));  // Converts to string
        printf("%s\n", ip_str);                // Prints IP
    }
}

// Prints all available network interfaces
// Input: None
// Output: None; prints interface names to stdout
void print_interfaces(void) {
    pcap_if_t *alldevs, *dev;              // Structures for pcap device list
    char errbuf[PCAP_ERRBUF_SIZE];         // Buffer for pcap errors
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {  // Retrieves all devices
        fprintf(stderr, "Error finding interfaces: %s\n", errbuf);
        return;
    }
    for (dev = alldevs; dev != NULL; dev = dev->next) {  // Iterates over devices
        printf("%s\n", dev->name);         // Prints interface name
    }
    pcap_freealldevs(alldevs);             // Frees device list
}

// Calculates checksum for a buffer (used in packet headers)
// Input: b (buffer pointer), len (buffer length in bytes)
// Output: 16-bit checksum value
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;               // Casts buffer to 16-bit words
    unsigned int sum = 0;                  // Running sum
    unsigned short result;
    while (len > 1) {                      // Processes 16-bit words
        sum += *buf++;
        len -= 2;
    }
    if (len == 1) sum += *(unsigned char *)buf;  // Handles odd byte
    sum = (sum >> 16) + (sum & 0xFFFF);   // Folds 32-bit sum to 16 bits
    sum += (sum >> 16);
    result = ~sum;                         // Computes one's complement
    return result;                         // Returns checksum
}

// Gets the IP address of a network interface for the specified family
// Input: interface (interface name), source_ip (buffer for IP string), len (buffer length), family (AF_INET/AF_INET6)
// Output: 0 on success, -1 on failure; fills source_ip with IP address
int get_interface_ip(const char *interface, char *source_ip, size_t len, int family) {
    struct ifaddrs *ifaddr, *ifa;          // Structures for interface addresses

    if (getifaddrs(&ifaddr) == -1) {       // Retrieves all interface addresses
        fprintf(stderr, "Error: getifaddrs failed\n");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {  // Iterates over interfaces
        if (ifa->ifa_addr == NULL || strcmp(ifa->ifa_name, interface) != 0) {  // Skips if no address or wrong interface
            continue;
        }

        if (ifa->ifa_addr->sa_family == family) {  // Matches requested family
            if (family == AF_INET) {       // IPv4 address
                struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                inet_ntop(AF_INET, &addr->sin_addr, source_ip, len);  // Converts to string
                freeifaddrs(ifaddr);       // Frees address list
                return 0;                  // Success
            } else if (family == AF_INET6) {  // IPv6 address
                struct sockaddr_in6 *addr = (struct sockaddr_in6 *)ifa->ifa_addr;
                if (!IN6_IS_ADDR_LINKLOCAL(&addr->sin6_addr)) {  // Skips link-local addresses
                    inet_ntop(AF_INET6, &addr->sin6_addr, source_ip, len);  // Converts to string
                    freeifaddrs(ifaddr);   // Frees address list
                    return 0;              // Success
                }
            }
        }
    }

    freeifaddrs(ifaddr);                   // Frees address list
    return -1;                             // Interface does not support requested family
}