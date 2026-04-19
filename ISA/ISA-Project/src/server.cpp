/**
 * @file server.cpp
 * @brief Implementation of DNS server network layer
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-17-10
 */

#include "../include/server.h"
#include "../include/logger.h"
#include "../include/query_handler.h"
#include "../include/socket_manager.h"
#include "../include/dns_protocol.h"
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <signal.h>
#include "../include/id_mapper.h"

// ============================================================================
// Global State for Signal Handling
// ============================================================================
static volatile bool server_running = true;

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number (SIGINT, SIGTERM)
 */
static void signal_handler(int signum) {
    LOG_INFO_F("Received signal %d, shutting down gracefully...", signum);
    server_running = false;
}

// ============================================================================
// Client Communication
// ============================================================================

std::string sockaddr_to_string(const sockaddr_storage& addr) {
    char addr_str[INET6_ADDRSTRLEN];
    char result[INET6_ADDRSTRLEN + 10];  // Extra space for port
    
    // Handle IPv4 addresses
    if (addr.ss_family == AF_INET) {
        const auto* addr_in = reinterpret_cast<const sockaddr_in*>(&addr);
        // Convert IPv4 address to string
        if (inet_ntop(AF_INET, &addr_in->sin_addr, addr_str, INET_ADDRSTRLEN)) {
            // Format as "IP:port"
            snprintf(result, sizeof(result), "%s:%u", addr_str, ntohs(addr_in->sin_port));
            return std::string(result);
        }
    } 

    // Handle IPv6 addresses
    else if (addr.ss_family == AF_INET6) {
        const auto* addr_in6 = reinterpret_cast<const sockaddr_in6*>(&addr);
        // Convert IPv6 address to string
        if (inet_ntop(AF_INET6, &addr_in6->sin6_addr, addr_str, INET6_ADDRSTRLEN)) {
            // Format as "[IPv6]:port"
            snprintf(result, sizeof(result), "[%s]:%u", addr_str, ntohs(addr_in6->sin6_port));
            return std::string(result);
        }
    }
    
    // Return fallback if conversion failed
    return "unknown";
}

bool send_response(int socket, const uint8_t* buffer, size_t len,
                const sockaddr_storage& client_addr, socklen_t addr_len) {
    
    // Send DNS response to client using sendto()
    ssize_t bytes_sent = sendto(socket, buffer, len, 0, 
                        (const sockaddr*)&client_addr, addr_len);
    
    // Check for send errors
    if (bytes_sent < 0) {
        std::string addr_str = sockaddr_to_string(client_addr);
        LOG_ERROR_F("Failed to send response to %s: %s", 
                addr_str.c_str(), strerror(errno));
        return false;
    }
    
    // Verify all bytes were sent
    if (static_cast<size_t>(bytes_sent) != len) {
        std::string addr_str = sockaddr_to_string(client_addr);
        LOG_WARNING_F("Partial send to %s: %zd/%zu bytes", 
                    addr_str.c_str(), bytes_sent, len);
        return false;
    }
    
    // Success - all bytes sent
    return true;
}

// ============================================================================
// Address Comparison for Spoofing Prevention
// ============================================================================

/**
 * @brief Compare two sockaddr_storage structures for equality
 * @param addr1 First address to compare
 * @param addr2 Second address to compare
 * @return true if addresses and ports match, false otherwise
 */
static bool compare_sockaddr(const sockaddr_storage& addr1, const sockaddr_storage& addr2) {
    // Address families must match
    if (addr1.ss_family != addr2.ss_family) {
        return false;
    }
    
    // Compare IPv4 addresses
    if (addr1.ss_family == AF_INET) {
        const auto* sin1 = reinterpret_cast<const sockaddr_in*>(&addr1);
        const auto* sin2 = reinterpret_cast<const sockaddr_in*>(&addr2);
        
        // Compare both IP address and port
        return (sin1->sin_addr.s_addr == sin2->sin_addr.s_addr) &&
               (sin1->sin_port == sin2->sin_port);
    } // Compare IPv6 addresses
    else if (addr1.ss_family == AF_INET6) {
        const auto* sin6_1 = reinterpret_cast<const sockaddr_in6*>(&addr1);
        const auto* sin6_2 = reinterpret_cast<const sockaddr_in6*>(&addr2);
        
        // Compare both IP address and port
        return (std::memcmp(&sin6_1->sin6_addr, &sin6_2->sin6_addr, sizeof(in6_addr)) == 0) &&
               (sin6_1->sin6_port == sin6_2->sin6_port);
    }
    
    // Unknown address family
    return false;
}

// ============================================================================
// Main Server Loop
// ============================================================================

int run_dns_server(const Config& config, const DomainFilter& filter) {
    LOG_INFO("Starting DNS server...");
    
    // -------------------- Signal Handler Setup --------------------
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // Termination request
    
    // -------------------- Socket Initialization --------------------
    // Create socket manager instance
    SocketManager socket_manager;
    
    // Initialize listening socket for client queries
    if (!socket_manager.initialize_listening(config.port)) {
        LOG_ERROR("Failed to create listening socket");
        return EXIT_FAILURE_CODE;
    }
    
    // Initialize upstream socket for forwarding to resolver
    if (!socket_manager.initialize_upstream(config.server_addr.ss_family)) {
        LOG_ERROR("Failed to create upstream socket");
        return EXIT_FAILURE_CODE;
    }
    
    // Get socket file descriptors for poll
    int listen_socket = socket_manager.get_listen_fd();
    int upstream_socket = socket_manager.get_upstream_fd();

    // -------------------- ID Mapper Initialization --------------------
    id_mapper_init();

    // -------------------- Upstream Address Configuration --------------------
    // Pre-resolve upstream server address
    sockaddr_storage upstream_addr = config.server_addr;

    // Determine length of upstream address structure
    socklen_t upstream_addr_len = config.server_addr_len;
    
    // Configure IPv4 upstream resolver
    if (upstream_addr.ss_family == AF_INET) {
        // Cast generic storage to IPv4-specific structure
        auto* sin = reinterpret_cast<sockaddr_in*>(&upstream_addr);

        // Use standard DNS port 53 if no port was specified during resolution
        if (sin->sin_port == 0) sin->sin_port = htons(DNS_DEFAULT_PORT);

        // Set structure size for IPv4 addresses (16 bytes)
        upstream_addr_len = sizeof(sockaddr_in);
    } 
    // Configure IPv6 upstream resolver
    else if (upstream_addr.ss_family == AF_INET6) {
        // Cast generic storage to IPv6-specific structure
        auto* sin6 = reinterpret_cast<sockaddr_in6*>(&upstream_addr);

        // Use standard DNS port 53 if no port was specified during resolution
        if (sin6->sin6_port == 0) sin6->sin6_port = htons(DNS_DEFAULT_PORT);
        
        // Set structure size for IPv6 addresses (28 bytes)
        upstream_addr_len = sizeof(sockaddr_in6);
    }
    
    LOG_INFO("DNS server ready - waiting for queries...");
    
    // -------------------- Poll Setup --------------------
    // Set up poll file descriptors for monitoring both sockets
    struct pollfd poll_fds[2];
    
    // Monitor listening socket for incoming client queries
    poll_fds[0].fd = listen_socket;
    poll_fds[0].events = POLLIN;  
    
    // Monitor upstream socket for responses from resolver
    poll_fds[1].fd = upstream_socket;
    poll_fds[1].events = POLLIN;  
    
    // -------------------- Main Event Loop --------------------
    while (server_running) {
        // Poll with 1-second timeout for periodic cleanup
        int ready = poll(poll_fds, 2, 1000);
        
        // Handle poll errors
        if (ready < 0) {
            if (errno == EINTR) {
                // Interrupted by signal - check server_running flag
                continue;
            }
            // Other poll errors
            LOG_ERROR_F("Poll failed: %s", strerror(errno));
            break;
        }
        
        // Handle poll timeout (no events)
        if (ready == 0) {
            // Timeout - opportunity for periodic tasks
            LOG_INFO("Poll timeout - server still running");
            // Cleanup stale ID mappings using configured timeout
            id_mapper_cleanup(config.timeout);
            continue;
        }
        
        // Check if listening socket has data ready (client queries)
        if (poll_fds[0].revents & POLLIN) {
            // Buffer and structures for receiving client query
            uint8_t query_buffer[DNS_BUFFER_SIZE];           // Buffer to store incoming DNS query
            sockaddr_storage client_addr;                    // Client's address (IPv4 or IPv6)
            socklen_t client_addr_len = sizeof(client_addr); // Size of address structure

            // Receive query
            ssize_t received = recvfrom(listen_socket, query_buffer, sizeof(query_buffer), 0,
                                (sockaddr*)&client_addr, &client_addr_len);

            // Handle receive errors
            if (received < 0) {
                // Error receiving data - ignore interrupted system calls
                if (errno != EINTR) LOG_ERROR_F("Failed to receive client query: %s", strerror(errno));
            } else if (received == 0) {
                // Empty packet received - log warning and continue
                LOG_WARNING("Received empty packet from client");
            } else {
                // -------------------- Buffer Size Validation --------------------
                if (static_cast<size_t>(received) > DNS_BUFFER_SIZE) {
                    LOG_ERROR_F("BUG: Received %zd bytes exceeds buffer size %zu", 
                                received, DNS_BUFFER_SIZE);
                    continue;
                }
                
                // Convert client address to human-readable string for logging
                std::string addr_str = sockaddr_to_string(client_addr);
                
                // -------------------- Query Processing --------------------
                QueryDecision decision = handle_query(
                    query_buffer, 
                    static_cast<size_t>(received), 
                    filter, 
                    addr_str
                );
                
                // -------------------- Query Decision Handling --------------------
                if (decision.action == QueryAction::FORWARD) {
                    // Query is allowed - forward to upstream resolver
                    
                    // Create mapping entry for this client query
                    ForwardEntry forward_entry;
                    forward_entry.client_addr = client_addr;
                    forward_entry.client_addr_len = client_addr_len;
                    forward_entry.orig_id = decision.parsed_msg.id;
                    forward_entry.query_len = static_cast<size_t>(received);

                    // Allocate a unique forwarded ID to avoid collisions
                    uint16_t forwarded_id = id_mapper_allocate(forward_entry);
                    if (forwarded_id == 0) {
                        LOG_ERROR("Failed to allocate forwarded ID; dropping query");
                        continue;
                    }

                    // Create a copy of the query buffer for forwarding
                    uint8_t forwarded_buffer[DNS_BUFFER_SIZE];
                    if (static_cast<size_t>(received) > sizeof(forwarded_buffer)) {
                        LOG_ERROR("Query too large to forward");
                        continue;
                    }

                    // Copy original query into forwarding buffer
                    std::memcpy(forwarded_buffer, query_buffer, received);
                    
                    // Rewrite the DNS ID field with our forwarded ID
                    *reinterpret_cast<uint16_t*>(forwarded_buffer) = htons(forwarded_id);

                    // Send to upstream resolver
                    ssize_t bytes_sent = sendto(upstream_socket, forwarded_buffer, received, 0,
                                    (const sockaddr*)&upstream_addr, upstream_addr_len);

                    // Check for send errors
                    if (bytes_sent < 0) {
                        LOG_ERROR_F("Failed to forward query to upstream: %s", strerror(errno));
                        // Remove the mapping we just created
                        ForwardEntry popped_entry;
                        id_mapper_pop(forwarded_id, popped_entry);
                    } else {
                        LOG_INFO_F("Forwarded query id=%u to upstream (%zd bytes)", forwarded_id, bytes_sent);
                    }
                } else {
                    // Query blocked or error - send pre-built response
                    if (decision.response_len > 0) {
                        send_response(listen_socket, 
                                    decision.response_buffer, 
                                    decision.response_len, 
                                    client_addr, 
                                    client_addr_len);
                    } else {
                        LOG_WARNING("No response built for error query");
                    }
                }
            }
        }
        
        // -------------------- Upstream Response Handling --------------------
        if (poll_fds[1].revents & POLLIN) {
            // Buffer to receive response from upstream resolver
            uint8_t response_buffer[DNS_BUFFER_SIZE];   // Buffer for upstream response
            sockaddr_storage from_addr;                 // Address of upstream server
            socklen_t from_len = sizeof(from_addr);     // Size of address structure
            
            // Receive response from upstream resolver
            ssize_t bytes_received = recvfrom(upstream_socket, response_buffer, sizeof(response_buffer), 0,
                                (sockaddr*)&from_addr, &from_len);
            
            if (bytes_received < 0) {
                // Error receiving response from upstream
                LOG_ERROR_F("Failed to receive from upstream: %s", strerror(errno));
            } else if (bytes_received == 0) {
                // Empty response received
                LOG_WARNING("Received empty packet from upstream");
            } else {
                // -------------------- Buffer Size Validation --------------------
                // Sanity check: ensure received size doesn't exceed buffer capacity
                if (static_cast<size_t>(bytes_received) > DNS_BUFFER_SIZE) {
                    LOG_ERROR_F("BUG: Received %zd bytes exceeds buffer size %zu", 
                                bytes_received, DNS_BUFFER_SIZE);
                    continue;
                }
                
                // -------------------- Verify Upstream Source (Anti-Spoofing) --------------------
                if (!compare_sockaddr(from_addr, upstream_addr)) {
                    std::string expected = sockaddr_to_string(upstream_addr);
                    std::string received = sockaddr_to_string(from_addr);
                    LOG_WARNING_F("Rejected spoofed response from %s (expected %s)", 
                                received.c_str(), expected.c_str());
                    continue;  // Drop packet and continue event loop
                }
                
                // Extract forwarded ID from response header and lookup original client
                if (bytes_received < static_cast<ssize_t>(DNS_HEADER_SIZE)) {
                    LOG_WARNING("Upstream response too short");
                } else {
                    // -------------------- Verify QR Bit (Response Validation) --------------------
                    // Verify QR bit is set (this must be a DNS response, not a query)
                    uint16_t flags = ntohs(*reinterpret_cast<uint16_t*>(response_buffer + 2));
                    if (!(flags & DNS_FLAG_QR)) {
                        LOG_WARNING("Rejected packet from upstream: QR bit not set (not a response)");
                        continue;  // Drop packet and continue event loop
                    }
                    
                    // -------------------- Validate Header Counts --------------------
                    // Extract and validate DNS header counts for sanity checking
                    const DNSHeader* header = reinterpret_cast<const DNSHeader*>(response_buffer);
                    uint16_t qdcount = ntohs(header->qdcount);
                    uint16_t ancount = ntohs(header->ancount);
                    uint16_t nscount = ntohs(header->nscount);
                    uint16_t arcount = ntohs(header->arcount);
                    
                    // QDCOUNT must be 1 (we only send single-question queries)
                    if (qdcount != 1) {
                        LOG_WARNING_F("Rejected response: invalid QDCOUNT=%u (expected 1)", qdcount);
                        continue;
                    }
                    
                    // Validate record counts are within reasonable limits
                    if (ancount > DNS_MAX_ANCOUNT || nscount > DNS_MAX_NSCOUNT || arcount > DNS_MAX_ARCOUNT) {
                        LOG_WARNING_F("Rejected response: unreasonable counts (AN=%u, NS=%u, AR=%u)", 
                                    ancount, nscount, arcount);
                        continue;
                    }
                    
                    // Get the forwarded ID from DNS header (first 2 bytes)
                    uint16_t forwarded_id = ntohs(*reinterpret_cast<uint16_t*>(response_buffer));
                    
                    // Look up the original client mapping using forwarded ID
                    ForwardEntry forward_entry;
                    if (!id_mapper_pop(forwarded_id, forward_entry)) {
                        // No mapping found - possibly stale or invalid response
                        LOG_WARNING_F("No mapping for forwarded ID %u", forwarded_id);
                    } else {
                        // Restore the original client's DNS ID in response header
                        *reinterpret_cast<uint16_t*>(response_buffer) = htons(forward_entry.orig_id);
                        
                        // Send response back to the original client
                        bool send_success = send_response(listen_socket, response_buffer, static_cast<size_t>(bytes_received), 
                                    forward_entry.client_addr, forward_entry.client_addr_len);
                        if (send_success) {
                            LOG_INFO_F("Relayed response for orig_id=%u to client", forward_entry.orig_id);
                        }
                    }
                }
            }
        }

        // Handle poll errors
        if ((poll_fds[0].revents | poll_fds[1].revents) & (POLLERR | POLLHUP | POLLNVAL)) {
            LOG_ERROR("Socket error detected in poll");
            break;
        }
    } // End of while (server_running)
    
    // -------------------- Cleanup --------------------
    LOG_INFO("Shutting down DNS server...");
    socket_manager.shutdown();  
    id_mapper_shutdown();
    LOG_INFO("DNS server stopped");
    
    return EXIT_SUCCESS_CODE;
}