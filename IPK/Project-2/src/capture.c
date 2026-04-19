#define _POSIX_C_SOURCE 200809L  // Ensures POSIX compatibility (e.g., for strdup, pcap functions)
#include <stdio.h>               // For fprintf, printf
#include <stdlib.h>              // For malloc, free, exit
#include <string.h>              // For strcmp, strcpy, strncat, strlen
#include <arpa/inet.h>           // For inet_ntop
#include <netinet/in.h>          // For sockaddr_in, sockaddr_in6
#include <netinet/ip.h>          // For struct ip (IPv4 header)
#include <netinet/ip6.h>         // For struct ip6_hdr (IPv6 header)
#include <netinet/tcp.h>         // For struct tcphdr (TCP header)
#include <netinet/udp.h>         // For struct udphdr (UDP header)
#include <netinet/icmp6.h>       // For struct icmp6_hdr (ICMPv6 header)
#include <netinet/ip_icmp.h>     // For struct icmp (ICMP header)
#include <pcap/pcap.h>           // For pcap functions and structures
#include "scanner.h"             // For PortStatus, capture_data, stop_capture, function prototypes
#include "utils.h"               // For INET6_ADDRSTRLEN

// Defines ICMP6_DST_UNREACH_PORT if not already defined (value 4 for "Port Unreachable")
#ifndef ICMP6_DST_UNREACH_PORT
#define ICMP6_DST_UNREACH_PORT 4
#endif

#define MAX_FILTER_SIZE 65536    // Arbitrary large size for filter string to accommodate many ports

// Packet handler callback for pcap_dispatch to process captured packets
// Input: user_data (capture_data struct), pkthdr (packet header), packet (raw packet data)
// Output: Updates port_status based on packet content
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct capture_data *data = (struct capture_data *)user_data;  // Casts user_data to capture_data
    PortStatus *port_status = data->port_status;                   // Array of port statuses
    int total_ports = data->total_ports;                           // Total number of ports to scan
    pcap_t *handle = data->handle;                                 // Pcap handle for capturing

    // Checks if capture should stop (e.g., due to Ctrl+C)
    if (stop_capture) {
        pcap_breakloop(handle);                                    // Breaks the pcap loop
        return;
    }

    // Determines link-layer type to calculate offset to IP header
    int linktype = pcap_datalink(handle);
    int offset = 0;
    switch (linktype) {
        case DLT_EN10MB: offset = 14; break;                       // Ethernet: 14-byte header
        case DLT_NULL: offset = 4; break;                         // Loopback: 4-byte header
        default:                                                  // Unsupported link type
            fprintf(stderr, "Unsupported link type: %d\n", linktype);
            return;
    }

    // Ensures packet is long enough to contain IP header
    if (pkthdr->caplen < offset + sizeof(struct ip)) {
        return;
    }

    const u_char *ip_packet = packet + offset;                     // Points to IP header
    char src_ip_str[INET6_ADDRSTRLEN], dst_ip_str[INET6_ADDRSTRLEN];  // Buffers for IP strings

    // Processes IPv4 packets
    if (((struct ip *)ip_packet)->ip_v == 4) {
        const struct ip *iph = (struct ip *)ip_packet;             // Casts to IPv4 header
        inet_ntop(AF_INET, &iph->ip_src, src_ip_str, sizeof(src_ip_str));  // Source IP to string
        inet_ntop(AF_INET, &iph->ip_dst, dst_ip_str, sizeof(dst_ip_str));  // Dest IP to string

        // Handles TCP packets
        if (iph->ip_p == IPPROTO_TCP) {
            if (pkthdr->caplen < offset + iph->ip_hl * 4 + sizeof(struct tcphdr)) return;  // Checks packet length
            const struct tcphdr *tcph = (struct tcphdr *)(ip_packet + (iph->ip_hl * 4));   // TCP header
            int dst_port = ntohs(tcph->th_dport);                      // Destination port (network to host order)
            for (int i = 0; i < total_ports; i++) {                    // Iterates over all ports
                if (strcmp(port_status[i].protocol, "tcp") == 0 &&     // Matches TCP protocol
                    port_status[i].src_port == dst_port &&             // Matches source port sent
                    strcmp(port_status[i].dest_ip, src_ip_str) == 0 && // Matches destination IP
                    !port_status[i].responded) {                       // Only if not yet responded
                    if ((tcph->th_flags & (TH_SYN | TH_ACK)) == (TH_SYN | TH_ACK)) {  // SYN-ACK response
                        strcpy(port_status[i].status, "open");         // Port is open
                        port_status[i].responded = 1;                  // Marks as responded
                    } else if (tcph->th_flags & TH_RST) {              // RST response
                        strcpy(port_status[i].status, "closed");       // Port is closed
                        port_status[i].responded = 1;                  // Marks as responded
                    }
                    break;                                             // Stops searching after match
                }
            }
        } 
        // Handles ICMP packets (for UDP)
        else if (iph->ip_p == IPPROTO_ICMP) {
            const struct icmp *icmph = (struct icmp *)(ip_packet + (iph->ip_hl * 4));  // ICMP header
            if (icmph->icmp_type == ICMP_UNREACH && icmph->icmp_code == ICMP_UNREACH_PORT) {  // Port Unreachable
                const struct ip *orig_iph = (struct ip *)(ip_packet + (iph->ip_hl * 4) + 8);  // Original IP header
                const struct udphdr *orig_udph = (struct udphdr *)(ip_packet + (iph->ip_hl * 4) + 8 + (orig_iph->ip_hl * 4));  // Original UDP header
                int orig_dst_port = ntohs(orig_udph->uh_dport);        // Original destination port
                inet_ntop(AF_INET, &orig_iph->ip_src, dst_ip_str, sizeof(dst_ip_str));  // Original source IP
                for (int i = 0; i < total_ports; i++) {
                    if (strcmp(port_status[i].protocol, "udp") == 0 && // Matches UDP protocol
                        port_status[i].port == orig_dst_port &&        // Matches target port
                        strcmp(port_status[i].dest_ip, dst_ip_str) == 0 &&  // Matches destination IP
                        !port_status[i].responded) {                   // Only if not yet responded
                        strcpy(port_status[i].status, "closed");       // Port is closed
                        port_status[i].responded = 1;                  // Marks as responded
                        break;
                    }
                }
            }
        }
    } 
    // Processes IPv6 packets
    else if (((struct ip6_hdr *)ip_packet)->ip6_vfc >> 4 == 6) {
        const struct ip6_hdr *ip6h = (struct ip6_hdr *)ip_packet;      // Casts to IPv6 header
        inet_ntop(AF_INET6, &ip6h->ip6_src, src_ip_str, sizeof(src_ip_str));  // Source IP to string
        inet_ntop(AF_INET6, &ip6h->ip6_dst, dst_ip_str, sizeof(dst_ip_str));  // Dest IP to string

        // Handles TCP packets
        if (ip6h->ip6_nxt == IPPROTO_TCP) {
            if (pkthdr->caplen < offset + sizeof(struct ip6_hdr) + sizeof(struct tcphdr)) return;  // Checks packet length
            const struct tcphdr *tcph = (struct tcphdr *)(ip_packet + sizeof(struct ip6_hdr));  // TCP header
            int dst_port = ntohs(tcph->th_dport);                      // Destination port
            for (int i = 0; i < total_ports; i++) {
                if (strcmp(port_status[i].protocol, "tcp") == 0 &&     // Matches TCP protocol
                    port_status[i].src_port == dst_port &&             // Matches source port sent
                    strcmp(port_status[i].dest_ip, src_ip_str) == 0 && // Matches destination IP
                    !port_status[i].responded) {                       // Only if not yet responded
                    if ((tcph->th_flags & (TH_SYN | TH_ACK)) == (TH_SYN | TH_ACK)) {  // SYN-ACK response
                        strcpy(port_status[i].status, "open");         // Port is open
                        port_status[i].responded = 1;                  // Marks as responded
                    } else if (tcph->th_flags & TH_RST) {              // RST response
                        strcpy(port_status[i].status, "closed");       // Port is closed
                        port_status[i].responded = 1;                  // Marks as responded
                    }
                    break;
                }
            }
        } 
        // Handles ICMPv6 packets (for UDP)
        else if (ip6h->ip6_nxt == IPPROTO_ICMPV6) {
            const struct icmp6_hdr *icmp6h = (struct icmp6_hdr *)(ip_packet + sizeof(struct ip6_hdr));  // ICMPv6 header
            if (icmp6h->icmp6_type == ICMP6_DST_UNREACH && icmp6h->icmp6_code == ICMP6_DST_UNREACH_PORT) {  // Port Unreachable
                const struct ip6_hdr *orig_ip6h = (struct ip6_hdr *)(ip_packet + sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr));  // Original IP header
                const struct udphdr *orig_udph = (struct udphdr *)(ip_packet + sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr) + sizeof(struct ip6_hdr));  // Original UDP header
                int orig_dst_port = ntohs(orig_udph->uh_dport);        // Original destination port
                inet_ntop(AF_INET6, &orig_ip6h->ip6_src, dst_ip_str, sizeof(dst_ip_str));  // Original source IP
                for (int i = 0; i < total_ports; i++) {
                    if (strcmp(port_status[i].protocol, "udp") == 0 && // Matches UDP protocol
                        port_status[i].port == orig_dst_port &&        // Matches target port
                        strcmp(port_status[i].dest_ip, dst_ip_str) == 0 &&  // Matches destination IP
                        !port_status[i].responded) {                   // Only if not yet responded
                        strcpy(port_status[i].status, "closed");       // Port is closed
                        port_status[i].responded = 1;                  // Marks as responded
                        break;
                    }
                }
            }
        }
    }
}

// Initializes packet capture on the specified interface with a filter
// Input: handle (pcap handle pointer), interface, port_status array, total_ports, ip_list, timeout
// Output: 0 on success, -1 on failure; sets up pcap handle
int capture_responses(pcap_t **handle, const char *interface, PortStatus *port_status, int total_ports, const IpList *ip_list, int timeout) {
    char errbuf[PCAP_ERRBUF_SIZE];                                 // Buffer for pcap error messages
    *handle = pcap_open_live(interface, BUFSIZ, 0, timeout, errbuf);  // Opens live capture on interface
    if (!*handle) {
        fprintf(stderr, "Error: pcap_open_live failed: %s\n", errbuf);
        return -1;
    }

    // Dynamicky alokuj filter s dostatočnou veľkosťou
    char *filter = malloc(MAX_FILTER_SIZE);
    if (!filter) {
        fprintf(stderr, "Memory allocation error\n");
        pcap_close(*handle);
        return -1;
    }
    filter[0] = '\0';                                              // Inicializuje prázdny filter

    snprintf(filter, MAX_FILTER_SIZE, "(host ");                   // Starts filter with host clause
    for (int i = 0; i < ip_list->count; i++) {                     // Adds each IP to filter
        char ip_str[INET6_ADDRSTRLEN];
        inet_ntop(ip_list->families[i],                            // Converts IP to string
                  ip_list->families[i] == AF_INET ? 
                  (void *)&((struct sockaddr_in *)&ip_list->ip_addresses[i])->sin_addr :
                  (void *)&((struct sockaddr_in6 *)&ip_list->ip_addresses[i])->sin6_addr, 
                  ip_str, sizeof(ip_str));
        strncat(filter, ip_str, MAX_FILTER_SIZE - strlen(filter) - 1);  // Appends IP to filter
        if (i < ip_list->count - 1) strncat(filter, " or host ", MAX_FILTER_SIZE - strlen(filter) - 1);  // Adds OR for multiple IPs
    }
    strncat(filter, ")", MAX_FILTER_SIZE - strlen(filter) - 1);    // Closes host clause

    int tcp_count = 0;
    int udp_count = 0;
    for (int i = 0; i < total_ports; i++) {                        // Counts TCP and UDP ports
        if (strcmp(port_status[i].protocol, "tcp") == 0) tcp_count++;
        else if (strcmp(port_status[i].protocol, "udp") == 0) udp_count++;
    }

    // Ak sú TCP alebo UDP porty, pridaj podmienky s 'and'
    if (tcp_count > 0 || udp_count > 0) {
        strncat(filter, " and (", MAX_FILTER_SIZE - strlen(filter) - 1);  // Opens protocol conditions

        if (tcp_count > 0) {                                       // If there are TCP ports
            strncat(filter, "tcp and (", MAX_FILTER_SIZE - strlen(filter) - 1);  // Opens TCP clause
            int current_tcp = 0;
            for (int i = 0; i < total_ports; i++) {                // Adds TCP source ports to filter
                if (strcmp(port_status[i].protocol, "tcp") == 0) {
                    char port_str[16];
                    snprintf(port_str, sizeof(port_str), "dst port %d", port_status[i].src_port);  // Adds dst port
                    if (strlen(filter) + strlen(port_str) + 4 < MAX_FILTER_SIZE - 1) {  // Kontrola dĺžky
                        strncat(filter, port_str, MAX_FILTER_SIZE - strlen(filter) - 1);
                        current_tcp++;
                        if (current_tcp < tcp_count) {             // Adds OR if not the last TCP port
                            strncat(filter, " or ", MAX_FILTER_SIZE - strlen(filter) - 1);
                        }
                    } else {
                        fprintf(stderr, "Error: Filter size exceeded\n");
                        free(filter);
                        pcap_close(*handle);
                        return -1;
                    }
                }
            }
            strncat(filter, ")", MAX_FILTER_SIZE - strlen(filter) - 1);  // Closes TCP ports
            if (udp_count > 0) {
                strncat(filter, " or ", MAX_FILTER_SIZE - strlen(filter) - 1);  // Adds OR between TCP and UDP
            }
        }

        if (udp_count > 0) {                                       // If there are UDP ports
            if (tcp_count == 0) {
                strncat(filter, "(", MAX_FILTER_SIZE - strlen(filter) - 1);  // Opens ICMP clause if no TCP
            }
            strncat(filter, "icmp[icmptype] == 3 or icmp6[icmp6type] == 1", MAX_FILTER_SIZE - strlen(filter) - 1);  // Adds ICMP clauses
            if (tcp_count == 0) {
                strncat(filter, ")", MAX_FILTER_SIZE - strlen(filter) - 1);  // Closes ICMP clause if no TCP
            }
        }

        strncat(filter, ")", MAX_FILTER_SIZE - strlen(filter) - 1);  // Closes protocol conditions
    }

    struct bpf_program fp;                                         // Compiled filter program
    if (pcap_compile(*handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN) < 0) {  // Compiles filter
        fprintf(stderr, "Error: pcap_compile failed: %s\n", pcap_geterr(*handle));
        free(filter);
        pcap_close(*handle);
        return -1;
    }

    if (pcap_setfilter(*handle, &fp) < 0) {                        // Applies filter to capture
        fprintf(stderr, "Error: pcap_setfilter failed: %s\n", pcap_geterr(*handle));
        pcap_freecode(&fp);
        free(filter);
        pcap_close(*handle);
        return -1;
    }
    pcap_freecode(&fp);                                            // Frees compiled filter
    free(filter);                                                  // Frees dynamically allocated filter

    return 0;                                                      // Success
}