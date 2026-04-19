#define _POSIX_C_SOURCE 200809L  // Ensures POSIX compatibility
#include <stdio.h>               // For printf, fprintf
#include <stdlib.h>              // For malloc, free
#include <string.h>              // For strcmp, strcpy, strncpy, strerror
#include <poll.h>                // For poll function and struct pollfd
#include <sys/time.h>            // For gettimeofday, struct timeval
#include <signal.h>              // For signal, SIGINT
#include <unistd.h>              // For close
#include <sys/socket.h>          // For socket, bind, getsockname
#include <arpa/inet.h>           // For inet_ntop
#include <errno.h>               // For errno
#include "scanner.h"             // For PortStatus, capture_data, stop_capture, function prototypes
#include "utils.h"               // For IpList, get_interface_ip
#include "packet.h"              // For send_tcp_syn_packet, send_udp_packet

volatile int stop_capture = 0;   // Global flag to stop capture (set by signal handler)

// Signal handler for SIGINT (Ctrl+C) to stop the capture loop
// Input: Signal number (ignored)
// Output: Sets stop_capture to 1
void signal_handler(int sig) {
    (void)sig;                   // Suppresses unused parameter warning
    stop_capture = 1;            // Signals the capture loop to stop
}

// Gets a free ephemeral port for the given address family
// Input: family (AF_INET or AF_INET6)
// Output: Free port number, or -1 on error
static int get_free_port(int family) {
    int sock = socket(family, SOCK_DGRAM, 0);  // Creates a UDP socket
    if (sock < 0) return -1;                   // Returns -1 if socket creation fails
    struct sockaddr_storage addr = {0};        // Storage for address
    addr.ss_family = family;                   // Sets address family
    socklen_t len = (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);  // Address length
    bind(sock, (struct sockaddr*)&addr, len);  // Binds to any available port
    getsockname(sock, (struct sockaddr*)&addr, &len);  // Retrieves assigned port
    int port = (family == AF_INET) ? ntohs(((struct sockaddr_in*)&addr)->sin_port) :  // Extracts port number
                                     ntohs(((struct sockaddr_in6*)&addr)->sin6_port);
    close(sock);                               // Closes the socket
    return port;                               // Returns the free port
}

// Main scanning function to perform TCP and UDP port scanning
// Input: args (command-line arguments), ip_list (resolved IP addresses)
// Output: 0 on success, -1 on failure; prints scan results to stdout
int scan(const Arguments *args, const IpList *ip_list) {
    const char *interface = args->interface;   // Network interface to use
    int total_ports = args->tcp_count + args->udp_count;  // Total number of ports to scan
    if (total_ports == 0) return 0;            // Returns if no ports specified

    if (ip_list->count == 0) {                 // Checks if any IPs were resolved
        fprintf(stderr, "Error: No IP addresses resolved for target\n");
        return -1;
    }

    // Allocates memory for port status array (one entry per IP per port)
    PortStatus *port_status = malloc(ip_list->count * total_ports * sizeof(PortStatus));
    if (!port_status) {
        fprintf(stderr, "Error: Memory allocation failed for port status\n");
        return -1;
    }

    int status_idx = 0;                        // Index for filling port_status array
    for (int i = 0; i < ip_list->count; i++) { // Iterates over resolved IPs
        int family = ip_list->families[i];     // Address family (IPv4 or IPv6)
        char dest_ip[INET6_ADDRSTRLEN];        // Buffer for destination IP string
        void *addr = (family == AF_INET) ?     // Extracts IP address based on family
                     (void *)&((struct sockaddr_in *)&ip_list->ip_addresses[i])->sin_addr :
                     (void *)&((struct sockaddr_in6 *)&ip_list->ip_addresses[i])->sin6_addr;
        inet_ntop(family, addr, dest_ip, sizeof(dest_ip));  // Converts IP to string

        char source_ip[INET6_ADDRSTRLEN];      // Buffer for source IP string
        if (get_interface_ip(interface, source_ip, sizeof(source_ip), family) < 0) {  // Gets interface IP
            fprintf(stderr, "Warning: Skipping %s - interface %s does not support %s\n",
                    dest_ip, interface, family == AF_INET ? "IPv4" : "IPv6");
            continue;
        }

        // Adds TCP ports to scan
        for (int j = 0; j < args->tcp_count; j++) {
            port_status[status_idx].port = args->tcp_ports[j];      // Target port
            port_status[status_idx].src_port = get_free_port(family);  // Source port
            strcpy(port_status[status_idx].protocol, "tcp");        // Protocol
            strcpy(port_status[status_idx].status, "filtered");     // Default status
            port_status[status_idx].responded = 0;                  // Not responded yet
            port_status[status_idx].attempts = 0;                   // No attempts yet
            strncpy(port_status[status_idx].dest_ip, dest_ip, INET6_ADDRSTRLEN);  // Dest IP
            strncpy(port_status[status_idx].source_ip, source_ip, INET6_ADDRSTRLEN);  // Source IP
            status_idx++;
        }
        // Adds UDP ports to scan
        for (int j = 0; j < args->udp_count; j++) {
            port_status[status_idx].port = args->udp_ports[j];      // Target port
            port_status[status_idx].src_port = get_free_port(family);  // Source port
            strcpy(port_status[status_idx].protocol, "udp");        // Protocol
            strcpy(port_status[status_idx].status, "open");         // Default status
            port_status[status_idx].responded = 0;                  // Not responded yet
            port_status[status_idx].attempts = 0;                   // No attempts yet
            strncpy(port_status[status_idx].dest_ip, dest_ip, INET6_ADDRSTRLEN);  // Dest IP
            strncpy(port_status[status_idx].source_ip, source_ip, INET6_ADDRSTRLEN);  // Source IP
            status_idx++;
        }
    }

    if (status_idx == 0) {                     // Checks if any ports were added
        fprintf(stderr, "Error: No compatible IP addresses for interface %s\n", interface);
        free(port_status);
        return -1;
    }

    pcap_t *handle = NULL;                     // Pcap handle for capturing
    int timeout_ms = args->timeout;            // Timeout from arguments

    // Initializes packet capture
    if (capture_responses(&handle, args->interface, port_status, status_idx, ip_list, timeout_ms) < 0) {
        free(port_status);
        return -1;
    }

    struct capture_data data = {port_status, status_idx, handle};  // Data for packet handler
    stop_capture = 0;                          // Resets stop flag
    signal(SIGINT, signal_handler);            // Sets up Ctrl+C handler

    // Sends initial packets for all ports
    for (int i = 0; i < status_idx; i++) {
        gettimeofday(&port_status[i].sent_time, NULL);  // Records send time
        port_status[i].attempts = 1;                    // Marks first attempt
        int family_idx = i / (args->tcp_count + args->udp_count);  // Index for IP family
        if (strcmp(port_status[i].protocol, "tcp") == 0) {  // Sends TCP SYN packet
            if (send_tcp_syn_packet(port_status[i].source_ip, port_status[i].dest_ip,
                                    port_status[i].src_port, port_status[i].port,
                                    ip_list->families[family_idx]) < 0) {  // Retries on failure
                port_status[i].src_port = get_free_port(ip_list->families[family_idx]);
                send_tcp_syn_packet(port_status[i].source_ip, port_status[i].dest_ip,
                                    port_status[i].src_port, port_status[i].port,
                                    ip_list->families[family_idx]);
            }
        } else if (strcmp(port_status[i].protocol, "udp") == 0) {  // Sends UDP packet
            if (send_udp_packet(port_status[i].source_ip, port_status[i].dest_ip,
                                port_status[i].src_port, port_status[i].port,
                                ip_list->families[family_idx]) < 0) {  // Retries on failure
                port_status[i].src_port = get_free_port(ip_list->families[family_idx]);
                send_udp_packet(port_status[i].source_ip, port_status[i].dest_ip,
                                port_status[i].src_port, port_status[i].port,
                                ip_list->families[family_idx]);
            }
        }
    }

    // Poll loop to wait for responses and handle timeouts
    struct pollfd pfd = { .fd = pcap_fileno(handle), .events = POLLIN };  // Poll descriptor for pcap
    while (!stop_capture) {                    // Continues until interrupted or all ports resolved
        struct timeval now;
        gettimeofday(&now, NULL);              // Current time for timeout calculation

        int next_timeout = timeout_ms;         // Next poll timeout
        int pending_ports = 0;                 // Number of ports still waiting for response

        // Waits for incoming packets
        int ret = poll(&pfd, 1, next_timeout);
        if (ret < 0) {                         // Poll error handling
            if (errno == EINTR) {              // Interrupted by signal (e.g., Ctrl+C)
                break;
            }
            fprintf(stderr, "Error: Poll failed during capture: %s\n", strerror(errno));
            break;
        }

        if (ret > 0 && (pfd.revents & POLLIN)) {  // Packets available
            int packet_count = pcap_dispatch(handle, 10, packet_handler, (u_char *)&data);  // Processes up to 10 packets
            if (packet_count < 0) {
                fprintf(stderr, "Error: pcap_dispatch failed: %s\n", pcap_geterr(handle));
                break;
            }
        }

        // Checks timeouts for each port
        for (int i = 0; i < status_idx; i++) {
            if (!port_status[i].responded && port_status[i].attempts > 0) {  // Still waiting
                long elapsed = (now.tv_sec - port_status[i].sent_time.tv_sec) * 1000 +
                               (now.tv_usec - port_status[i].sent_time.tv_usec) / 1000;  // Time since sent
                long remaining = timeout_ms - elapsed;  // Remaining time
                if (remaining <= 0) {                   // Timeout expired
                    if (strcmp(port_status[i].protocol, "tcp") == 0 && strcmp(port_status[i].status, "filtered") == 0) {
                        if (port_status[i].attempts == 1) {  // First attempt failed, retry
                            int family_idx = i / (args->tcp_count + args->udp_count);
                            gettimeofday(&port_status[i].sent_time, NULL);
                            port_status[i].attempts = 2;
                            if (send_tcp_syn_packet(port_status[i].source_ip, port_status[i].dest_ip,
                                                    port_status[i].src_port, port_status[i].port,
                                                    ip_list->families[family_idx]) < 0) {  // Retries on failure
                                port_status[i].src_port = get_free_port(ip_list->families[family_idx]);
                                send_tcp_syn_packet(port_status[i].source_ip, port_status[i].dest_ip,
                                                    port_status[i].src_port, port_status[i].port,
                                                    ip_list->families[family_idx]);
                            }
                        } else if (port_status[i].attempts == 2) {  // Second attempt failed
                            port_status[i].responded = 1;
                        }
                    } else if (strcmp(port_status[i].protocol, "udp") == 0) {  // UDP timeout
                        port_status[i].responded = 1;
                    }
                } else if (remaining < next_timeout) {  // Adjusts next poll timeout
                    next_timeout = remaining;
                }
                pending_ports++;                        // Counts pending ports
            }
        }

        if (pending_ports == 0) {                   // All ports resolved
            break;
        }
    }

    // Prints scan results in required format
    for (int i = 0; i < status_idx; i++) {
        printf("%s %d %s %s\n", port_status[i].dest_ip, port_status[i].port,
               port_status[i].protocol, port_status[i].status);
    }

    pcap_close(handle);                            // Closes pcap handle
    free(port_status);                             // Frees port status array
    return 0;                                      // Success
}