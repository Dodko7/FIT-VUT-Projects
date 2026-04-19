/**
 * @file server.h
 * @brief DNS server network layer and main event loop
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-17-10
 *
 * Responsibilities:
 * - Poll-based event loop monitoring two UDP sockets (listen + upstream)
 * - Receiving DNS queries from clients via recvfrom() on listen socket
 * - Forwarding allowed queries to upstream resolver with ID rewriting
 * - Relaying resolver responses to original clients with ID restoration
 * - Signal handling (SIGINT/SIGTERM) for graceful shutdown
 * - Integration with ID mapper for tracking forwarded queries
 * - Periodic cleanup of stale ID mappings using configurable timeout
 */

#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "arg_parser.h"
#include "filter.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

/**
 * @brief Convert sockaddr_storage to human-readable string representation
 * 
 * Uses inet_ntop() to convert network addresses to string format:
 * - IPv4: "192.0.2.1:12345"
 * - IPv6: "[2001:db8::1]:12345"
 * Handles both address families and includes port number in output.
 * Returns "unknown" if address family is not AF_INET or AF_INET6.
 * 
 * @param addr Socket address structure (IPv4 or IPv6)
 * @return String representation of address and port, or "unknown" on error
 */
std::string sockaddr_to_string(const sockaddr_storage& addr);

/**
 * @brief Send DNS response to client via UDP
 * 
 * Uses sendto() to transmit DNS response buffer to specified client address.
 * Validates that all bytes were sent (checks for partial sends).
 * Logs errors and warnings with client address information.
 * 
 * @param socket Socket file descriptor to send from (typically listen socket)
 * @param buffer DNS response message bytes
 * @param len Length of response buffer in bytes
 * @param client_addr Destination address structure (IPv4 or IPv6)
 * @param addr_len Size of client_addr structure (sizeof(sockaddr_in) or sockaddr_in6)
 * @return true if all bytes sent successfully, false on error or partial send
 */
bool send_response(int socket, const uint8_t* buffer, size_t len, 
                    const sockaddr_storage& client_addr, socklen_t addr_len);

/**
 * @brief Main DNS server event loop with poll-based networking
 * 
 * Server lifecycle:
 * 1. Install signal handlers (SIGINT, SIGTERM) for graceful shutdown
 * 2. Create SocketManager and initialize listen socket (bind to config.port) + upstream socket
 * 3. Initialize ID mapper for tracking forwarded queries
 * 4. Configure upstream resolver address (set DNS port 53 if needed)
 * 5. Enter poll() event loop monitoring both sockets with 1-second timeout
 * 6. On listen socket ready: receive client query, call handle_query() for decision
 *    - FORWARD: allocate ID, rewrite header, sendto() upstream, store mapping
 *    - BLOCKED/ERROR: send pre-built response directly to client
 * 7. On upstream socket ready: receive resolver response, lookup mapping by forwarded ID,
 *    restore original client ID, relay response to original client
 * 8. On poll timeout: cleanup stale ID mappings using config.timeout
 * 9. On shutdown signal: exit loop, cleanup sockets and ID mapper
 * 
 * Threading: Single-threaded; poll() handles multiplexing of two sockets.
 * 
 * @param config Server configuration (port, upstream resolver address, timeout)
 * @param filter Domain filter for query blocking decisions
 * @return EXIT_SUCCESS_CODE on clean shutdown, EXIT_FAILURE_CODE on fatal errors
 */
int run_dns_server(const Config& config, const DomainFilter& filter);

#endif // SERVER_H