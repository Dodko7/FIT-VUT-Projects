/**
 * @file socket_manager.cpp
 * @brief Implementation of socket lifecycle management
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-16-11
 */

#include "../include/socket_manager.h"
#include "../include/logger.h"
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>

// ============================================================================
// Constructor / Destructor
// ============================================================================

SocketManager::SocketManager() 
    : listen_socket(-1)
    , upstream_socket(-1) {
    // Initialize sockets to invalid state
}

SocketManager::~SocketManager() {
    // RAII - automatically clean up sockets
    shutdown();
}

// ============================================================================
// Socket Initialization
// ============================================================================

bool SocketManager::initialize_listening(uint16_t port) {
    LOG_INFO_F("Creating listening socket on port %u", port);
    
    // Create IPv6 socket that will handle both IPv4 and IPv6
    listen_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (listen_socket < 0) {
        LOG_ERROR_F("Failed to create socket: %s", strerror(errno));
        return false;
    }
    
    // Allow socket reuse to prevent "Address already in use" errors after restart
    int reuse = 1;
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LOG_WARNING_F("Failed to set SO_REUSEADDR: %s", strerror(errno));
        // Not critical, continue anyway
    }
    
    // Enable IPv4-mapped addresses on IPv6 socket (dual-stack)
    int no = 0;
    if (setsockopt(listen_socket, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0) {
        LOG_ERROR_F("Failed to disable IPV6_V6ONLY: %s", strerror(errno));
        close(listen_socket);
        listen_socket = -1;
        return false;
    }
    
    // Prepare address structure for binding
    sockaddr_in6 server_addr;
    
    // Zero-initialize the structure
    std::memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin6_family = AF_INET6;         // IPv6 address family
    server_addr.sin6_addr = in6addr_any;        // Listen on all interfaces
    server_addr.sin6_port = htons(port);        // Convert port to network order
    
    // Bind socket to the specified port
    if (bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        // Handle common bind errors with specific messages
        if (errno == EACCES) {
            LOG_ERROR_F("Cannot bind to port %u: Permission denied (try running as root)", port);
        } else if (errno == EADDRINUSE) {
            LOG_ERROR_F("Port %u already in use (another DNS server running?)", port);
        } else if (errno == EADDRNOTAVAIL) {
            LOG_ERROR_F("Cannot bind to port %u: Address not available", port);
        } else {
            LOG_ERROR_F("Failed to bind to port %u: %s", port, strerror(errno));
        }
        close(listen_socket);
        listen_socket = -1;
        return false;
    }
    
    // Log successful socket creation
    if (port < PRIVILEGED_PORT_MAX) {
        LOG_INFO_F("Successfully bound to privileged port %u (IPv4/IPv6)", port);
    } else {
        LOG_INFO_F("DNS server listening on port %u (IPv4/IPv6)", port);
    }
    
    return true;
}

bool SocketManager::initialize_upstream(sa_family_t addr_family) {
    LOG_INFO("Creating upstream socket for DNS forwarding");
    
    // Create UDP socket matching the upstream resolver's address family
    upstream_socket = socket(addr_family, SOCK_DGRAM, IPPROTO_UDP);
    if (upstream_socket < 0) {
        LOG_ERROR_F("Failed to create upstream socket: %s", strerror(errno));
        return false;
    }
    
    LOG_INFO("Upstream socket created successfully");
    return true;
}

// ============================================================================
// Cleanup
// ============================================================================

void SocketManager::shutdown() {
    // Close listening socket if open
    if (listen_socket >= 0) {
        LOG_INFO("Closing listening socket");
        close(listen_socket);
        listen_socket = -1;
    }
    
    // Close upstream socket if open
    if (upstream_socket >= 0) {
        LOG_INFO("Closing upstream socket");
        close(upstream_socket);
        upstream_socket = -1;
    }
}
