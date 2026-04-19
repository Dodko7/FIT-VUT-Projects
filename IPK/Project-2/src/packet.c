#define _POSIX_C_SOURCE 200809L  // Ensures POSIX compatibility (e.g., for socket functions)
#include <stdio.h>               // For fprintf
#include <stdlib.h>              // For malloc, free (not used directly but standard)
#include <string.h>              // For memcpy, strerror
#include <unistd.h>              // For close
#include <errno.h>               // For errno
#include <arpa/inet.h>           // For inet_addr, inet_pton, htons, htonl
#include <netinet/in.h>          // For struct sockaddr_in, sockaddr_in6, AF_INET, AF_INET6
#include <netinet/ip.h>          // For struct ip (IPv4 header)
#include <netinet/ip6.h>         // For struct ip6_hdr (IPv6 header, not used directly here)
#include <netinet/tcp.h>         // For struct tcphdr, TH_SYN
#include <netinet/udp.h>         // For struct udphdr
#include <sys/socket.h>          // For socket, sendto, setsockopt, bind
#include "packet.h"              // For function prototypes
#include "utils.h"               // For checksum function

// Pseudo-header structure for IPv4 TCP/UDP checksum calculation
struct pseudo_header {
    u_int32_t source_address;    // Source IP address
    u_int32_t dest_address;      // Destination IP address
    u_int8_t placeholder;        // Zero byte
    u_int8_t protocol;           // Protocol number (TCP/UDP)
    u_int16_t length;            // Length of TCP/UDP header
};

// Pseudo-header structure for IPv6 TCP/UDP checksum calculation
struct pseudo_header6 {
    struct in6_addr source_address;  // Source IPv6 address
    struct in6_addr dest_address;    // Destination IPv6 address
    u_int32_t length;                // Length of TCP/UDP header
    u_int8_t zero[3];                // Three zero bytes
    u_int8_t next_header;            // Next header (protocol number)
};

// Prepares a TCP header for a SYN packet with checksum
// Input: tcph (TCP header pointer), src_port, dst_port, source_ip, dest_ip, family (AF_INET/AF_INET6)
// Output: Fills tcph with appropriate values and calculates checksum
void prepare_tcp_header(struct tcphdr *tcph, int src_port, int dst_port, 
                        const char *source_ip, const char *dest_ip, int family) {
    tcph->th_sport = htons(src_port);       // Sets source port (host to network byte order)
    tcph->th_dport = htons(dst_port);       // Sets destination port
    tcph->th_seq = htonl(12345);            // Sets arbitrary sequence number
    tcph->th_ack = 0;                       // No acknowledgment number (SYN only)
    tcph->th_off = 5;                       // Data offset (5 words = 20 bytes, no options)
    tcph->th_flags = TH_SYN;                // Sets SYN flag for connection initiation
    tcph->th_win = htons(8192);             // Sets window size (arbitrary value)
    tcph->th_sum = 0;                       // Initializes checksum to 0

    if (family == AF_INET) {                // IPv4 checksum calculation
        struct pseudo_header psh = {
            .source_address = inet_addr(source_ip),  // Converts source IP to binary
            .dest_address = inet_addr(dest_ip),      // Converts destination IP to binary
            .placeholder = 0,                        // Zero byte
            .protocol = IPPROTO_TCP,                 // TCP protocol number
            .length = htons(sizeof(struct tcphdr))   // Length of TCP header
        };
        char pseudo_packet[sizeof(struct pseudo_header) + sizeof(struct tcphdr)];  // Buffer for pseudo-header + TCP header
        memcpy(pseudo_packet, &psh, sizeof(psh));       // Copies pseudo-header
        memcpy(pseudo_packet + sizeof(psh), tcph, sizeof(*tcph));  // Copies TCP header
        tcph->th_sum = checksum((unsigned short *)pseudo_packet, sizeof(pseudo_packet));  // Calculates checksum
    } else if (family == AF_INET6) {        // IPv6 checksum calculation
        struct pseudo_header6 psh6 = {0};   // Initializes pseudo-header to zero
        inet_pton(AF_INET6, source_ip, &psh6.source_address);  // Converts source IP to binary
        inet_pton(AF_INET6, dest_ip, &psh6.dest_address);      // Converts destination IP to binary
        psh6.length = htonl(sizeof(struct tcphdr));            // Length of TCP header
        psh6.next_header = IPPROTO_TCP;                        // TCP protocol number

        char pseudo_packet[sizeof(struct pseudo_header6) + sizeof(struct tcphdr)];  // Buffer for pseudo-header + TCP header
        memcpy(pseudo_packet, &psh6, sizeof(psh6));            // Copies pseudo-header
        memcpy(pseudo_packet + sizeof(psh6), tcph, sizeof(*tcph));  // Copies TCP header
        tcph->th_sum = checksum((unsigned short *)pseudo_packet, sizeof(pseudo_packet));  // Calculates checksum
    }
}

// Prepares a UDP header with checksum (optional for IPv4, mandatory for IPv6)
// Input: udph (UDP header pointer), src_port, dst_port, source_ip, dest_ip, family (AF_INET/AF_INET6)
// Output: Fills udph with appropriate values and calculates checksum for IPv6
void prepare_udp_header(struct udphdr *udph, int src_port, int dst_port, 
                        const char *source_ip, const char *dest_ip, int family) {
    udph->uh_sport = htons(src_port);       // Sets source port
    udph->uh_dport = htons(dst_port);       // Sets destination port
    udph->uh_ulen = htons(sizeof(struct udphdr));  // Sets length (header only, no data)
    udph->uh_sum = 0;                       // Initializes checksum to 0

    if (family == AF_INET6) {               // IPv6 requires checksum
        struct pseudo_header6 psh6 = {0};   // Initializes pseudo-header to zero
        inet_pton(AF_INET6, source_ip, &psh6.source_address);  // Converts source IP
        inet_pton(AF_INET6, dest_ip, &psh6.dest_address);      // Converts destination IP
        psh6.length = htonl(sizeof(struct udphdr));            // Length of UDP header
        psh6.next_header = IPPROTO_UDP;                        // UDP protocol number

        char pseudo_packet[sizeof(struct pseudo_header6) + sizeof(struct udphdr)];  // Buffer for pseudo-header + UDP header
        memcpy(pseudo_packet, &psh6, sizeof(psh6));            // Copies pseudo-header
        memcpy(pseudo_packet + sizeof(psh6), udph, sizeof(*udph));  // Copies UDP header
        udph->uh_sum = checksum((unsigned short *)pseudo_packet, sizeof(pseudo_packet));  // Calculates checksum
    } // IPv4 checksum is optional and omitted here for simplicity
}

// Sends a TCP SYN packet to the specified destination
// Input: source_ip, dest_ip, src_port, dst_port, family (AF_INET/AF_INET6)
// Output: 0 on success, -1 on failure
int send_tcp_syn_packet(const char *source_ip, const char *dest_ip, int src_port, int dst_port, int family) {
    int sock;
    if (family == AF_INET) {                // IPv4 implementation
        sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);  // Creates raw socket for full control
        if (sock < 0) {
            fprintf(stderr, "Error: Failed to create raw socket: %s\n", strerror(errno));
            return -1;
        }

        int one = 1;
        if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {  // Enables custom IP header
            fprintf(stderr, "Error: Failed to set IP_HDRINCL: %s\n", strerror(errno));
            close(sock);
            return -1;
        }

        struct ip iph = {0};                // Initializes IPv4 header
        iph.ip_v = 4;                       // Version 4
        iph.ip_hl = 5;                      // Header length (5 words = 20 bytes)
        iph.ip_tos = 0;                     // Type of service
        iph.ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr));  // Total length
        iph.ip_id = htons(54321);           // Arbitrary identification
        iph.ip_off = 0;                     // No fragmentation
        iph.ip_ttl = 64;                    // Time to live
        iph.ip_p = IPPROTO_TCP;             // TCP protocol
        iph.ip_sum = 0;                     // Initializes checksum
        inet_pton(AF_INET, source_ip, &iph.ip_src);  // Sets source IP
        inet_pton(AF_INET, dest_ip, &iph.ip_dst);    // Sets destination IP
        iph.ip_sum = checksum((unsigned short *)&iph, sizeof(iph));  // Calculates IP checksum

        struct tcphdr tcph;                 // TCP header
        prepare_tcp_header(&tcph, src_port, dst_port, source_ip, dest_ip, family);  // Prepares TCP header

        char packet[sizeof(struct ip) + sizeof(struct tcphdr)];  // Packet buffer
        memcpy(packet, &iph, sizeof(iph));       // Copies IP header
        memcpy(packet + sizeof(iph), &tcph, sizeof(tcph));  // Copies TCP header

        struct sockaddr_in dest_addr = {0};      // Destination address
        dest_addr.sin_family = AF_INET;          // IPv4
        dest_addr.sin_addr.s_addr = iph.ip_dst.s_addr;  // Destination IP

        ssize_t sent = sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));  // Sends packet
        if (sent < 0) {
            fprintf(stderr, "Error: Failed to send TCP SYN packet to %s:%d: %s\n", dest_ip, dst_port, strerror(errno));
            close(sock);
            return -1;
        }
    } else if (family == AF_INET6) {        // IPv6 implementation
        sock = socket(AF_INET6, SOCK_RAW, IPPROTO_TCP);  // Creates raw socket for TCP
        if (sock < 0) {
            fprintf(stderr, "Error: Failed to create raw socket for IPv6: %s\n", strerror(errno));
            return -1;
        }

        struct sockaddr_in6 src_addr = {0};  // Source address
        src_addr.sin6_family = AF_INET6;     // IPv6
        inet_pton(AF_INET6, source_ip, &src_addr.sin6_addr);  // Sets source IP
        src_addr.sin6_port = htons(src_port);  // Sets source port
        if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {  // Binds to source IP/port
            fprintf(stderr, "Error: Failed to bind source IP %s: %s\n", source_ip, strerror(errno));
            close(sock);
            return -1;
        }

        struct tcphdr tcph;                 // TCP header
        prepare_tcp_header(&tcph, src_port, dst_port, source_ip, dest_ip, family);  // Prepares TCP header

        struct sockaddr_in6 dest_addr = {0};  // Destination address
        dest_addr.sin6_family = AF_INET6;     // IPv6
        inet_pton(AF_INET6, dest_ip, &dest_addr.sin6_addr);  // Sets destination IP

        ssize_t sent = sendto(sock, &tcph, sizeof(tcph), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));  // Sends TCP header
        if (sent < 0) {
            fprintf(stderr, "Error: Failed to send TCP SYN packet (IPv6) to %s:%d: %s\n", dest_ip, dst_port, strerror(errno));
            close(sock);
            return -1;
        }
    }

    close(sock);                            // Closes socket
    return 0;                               // Success
}

// Sends a UDP packet to the specified destination
// Input: source_ip, dest_ip, src_port, dst_port, family (AF_INET/AF_INET6)
// Output: 0 on success, -1 on failure
int send_udp_packet(const char *source_ip, const char *dest_ip, int src_port, int dst_port, int family) {
    int sock = socket(family, SOCK_RAW, IPPROTO_UDP);  // Creates raw socket for UDP
    if (sock < 0) {
        fprintf(stderr, "Error: Failed to create raw socket for UDP: %s\n", strerror(errno));
        return -1;
    }

    struct udphdr udph;                     // UDP header
    prepare_udp_header(&udph, src_port, dst_port, source_ip, dest_ip, family);  // Prepares UDP header

    if (family == AF_INET) {                // IPv4 implementation
        struct sockaddr_in dest_addr = {0};  // Destination address
        dest_addr.sin_family = AF_INET;      // IPv4
        inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr);  // Sets destination IP

        ssize_t sent = sendto(sock, &udph, sizeof(udph), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));  // Sends UDP packet
        if (sent < 0) {
            fprintf(stderr, "Error: Failed to send UDP packet to %s:%d: %s\n", dest_ip, dst_port, strerror(errno));
            close(sock);
            return -1;
        }
    } else if (family == AF_INET6) {        // IPv6 implementation
        struct sockaddr_in6 src_addr = {0};  // Source address
        src_addr.sin6_family = AF_INET6;     // IPv6
        inet_pton(AF_INET6, source_ip, &src_addr.sin6_addr);  // Sets source IP
        src_addr.sin6_port = htons(src_port);  // Sets source port
        if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {  // Binds to source IP/port
            fprintf(stderr, "Error: Failed to bind source IP %s: %s\n", source_ip, strerror(errno));
            close(sock);
            return -1;
        }

        struct sockaddr_in6 dest_addr = {0};  // Destination address
        dest_addr.sin6_family = AF_INET6;     // IPv6
        inet_pton(AF_INET6, dest_ip, &dest_addr.sin6_addr);  // Sets destination IP

        ssize_t sent = sendto(sock, &udph, sizeof(udph), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));  // Sends UDP packet
        if (sent < 0) {
            fprintf(stderr, "Error: Failed to send UDP packet (IPv6) to %s:%d: %s\n", dest_ip, dst_port, strerror(errno));
            close(sock);
            return -1;
        }
    }
    close(sock);                            // Closes socket
    return 0;                               // Success
}