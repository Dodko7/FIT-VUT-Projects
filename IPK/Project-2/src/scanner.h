#ifndef SCANNER_H
#define SCANNER_H

#include <sys/time.h>
#include <pcap/pcap.h>
#include "utils.h"
#include "args.h"

// Structure to store information about a scanned port
typedef struct {
    int port;                   // Target port
    int src_port;               // Source port used for scanning
    char protocol[4];           // Protocol ("tcp" or "udp")
    char status[16];            // Port status ("open", "closed", "filtered")
    int responded;              // Response indicator (0 = no response, 1 = response received)
    int attempts;               // Number of attempts made to scan the port
    char dest_ip[INET6_ADDRSTRLEN];  // Target IP address
    char source_ip[INET6_ADDRSTRLEN]; // Source IP address
    struct timeval sent_time;   // Timestamp of when the packet was sent
} PortStatus;

// Structure to store data related to packet capturing
struct capture_data {
    PortStatus *port_status;    // Array of port statuses
    int total_ports;            // Total number of ports being scanned
    pcap_t *handle;             // Pcap handle for capturing packets
};

// Global variable to signal capture termination
extern volatile int stop_capture;

/**
 * Initiates a port scan based on the provided arguments.
 * @param args Pointer to the Arguments structure containing scan settings.
 * @param ip_list Pointer to the IpList structure with target IP addresses.
 * @return 0 on success, -1 on failure.
 */
int scan(const Arguments *args, const IpList *ip_list);

/**
 * Callback function for handling captured packets.
 * @param user_data Pointer to user-defined data (PortStatus structure).
 * @param pkthdr Pointer to the pcap packet header.
 * @param packet Pointer to the captured packet data.
 */
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet);

/**
 * Captures and processes responses from scanned targets.
 * @param handle_ptr Pointer to the pcap handle.
 * @param interface Network interface name to capture packets on.
 * @param port_status Array of port statuses being monitored.
 * @param total_ports Total number of ports being scanned.
 * @param ip_list Pointer to the IpList structure containing target IPs.
 * @param timeout Capture timeout in milliseconds.
 * @return 0 on success, -1 on failure.
 */
int capture_responses(pcap_t **handle_ptr, const char *interface, PortStatus *port_status,
                      int total_ports, const IpList *ip_list, int timeout);

#endif
