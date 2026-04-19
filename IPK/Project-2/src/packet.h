#ifndef PACKET_H
#define PACKET_H

#include <netinet/tcp.h>
#include <netinet/udp.h>

/**
 * Prepares a TCP header for a SYN packet.
 * @param tcph Pointer to the TCP header structure.
 * @param src_port Source port number.
 * @param dst_port Destination port number.
 * @param source_ip Source IP address as a string.
 * @param dest_ip Destination IP address as a string.
 * @param family Address family (AF_INET for IPv4, AF_INET6 for IPv6).
 */
void prepare_tcp_header(struct tcphdr *tcph, int src_port, int dst_port, 
                        const char *source_ip, const char *dest_ip, int family);

/**
 * Prepares a UDP header for a packet.
 * @param udph Pointer to the UDP header structure.
 * @param src_port Source port number.
 * @param dst_port Destination port number.
 * @param source_ip Source IP address as a string.
 * @param dest_ip Destination IP address as a string.
 * @param family Address family (AF_INET for IPv4, AF_INET6 for IPv6).
 */
void prepare_udp_header(struct udphdr *udph, int src_port, int dst_port, 
                        const char *source_ip, const char *dest_ip, int family);

/**
 * Sends a TCP SYN packet to initiate a connection.
 * @param source_ip Source IP address.
 * @param dest_ip Destination IP address.
 * @param src_port Source port number.
 * @param dst_port Destination port number.
 * @param family Address family (AF_INET for IPv4, AF_INET6 for IPv6).
 * @return 0 on success, -1 on failure.
 */
int send_tcp_syn_packet(const char *source_ip, const char *dest_ip, int src_port, int dst_port, int family);

/**
 * Sends a UDP packet to the specified destination.
 * @param source_ip Source IP address.
 * @param dest_ip Destination IP address.
 * @param src_port Source port number.
 * @param dst_port Destination port number.
 * @param family Address family (AF_INET for IPv4, AF_INET6 for IPv6).
 * @return 0 on success, -1 on failure.
 */
int send_udp_packet(const char *source_ip, const char *dest_ip, int src_port, int dst_port, int family);

#endif
