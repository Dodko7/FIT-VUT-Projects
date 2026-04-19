/**
 * @file socket_manager.h
 * @brief Socket lifecycle management for DNS server using RAII pattern
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-16-11
 * 
 * Responsibilities:
 * - Create IPv6 dual-stack listening socket (IPV6_V6ONLY=0) bound to in6addr_any
 * - Configure SO_REUSEADDR to prevent "address in use" errors on restart
 * - Create address-family-matched upstream socket (AF_INET or AF_INET6)
 * - Automatic socket cleanup via RAII destructor pattern
 * - Provide socket file descriptors for poll()-based event loop
 * - Handle common bind() errors (EACCES, EADDRINUSE, EADDRNOTAVAIL) with specific messages
 */

#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "common.h"
#include <netinet/in.h>
#include <sys/socket.h>

/**
 * @brief Manages socket lifecycle for DNS server with automatic cleanup
 * 
 * RAII-based socket manager that ensures sockets are always properly closed.
 * Creates dual-stack IPv6 listening socket and address-family-matched upstream socket.
 * Prevents socket copying to avoid double-close issues.
 */
class SocketManager {
public:
    /**
     * @brief Constructor - initializes socket descriptors to invalid state
     */
    SocketManager();
    
    /**
     * @brief Destructor - automatically closes all sockets
     */
    ~SocketManager();
    
    // Disable copy (sockets cannot be copied)
    SocketManager(const SocketManager&) = delete;
    SocketManager& operator=(const SocketManager&) = delete;
    
    /**
     * @brief Create and bind listening socket for incoming client queries
     * 
     * Implementation steps:
     * 1. Create IPv6 UDP socket (AF_INET6, SOCK_DGRAM, IPPROTO_UDP)
     * 2. Set SO_REUSEADDR to allow quick restart after shutdown
     * 3. Set IPV6_V6ONLY=0 to enable dual-stack (IPv4-mapped addresses)
     * 4. Bind to in6addr_any (all interfaces) on specified port
     * 5. Handle bind errors: EACCES (permission), EADDRINUSE (port busy), EADDRNOTAVAIL
     * 
     * Privileged ports (<1024) require root/CAP_NET_BIND_SERVICE.
     * 
     * @param port Port number to bind to (1-65535, typically 53 for DNS)
     * @return true if socket created and bound successfully, false on any error
     */
    bool initialize_listening(uint16_t port);
    
    /**
     * @brief Create upstream socket for forwarding queries to resolver
     * 
     * Creates UDP socket with address family matching upstream resolver.
     * Socket is unbound (not listening) - only used for sendto()/recvfrom().
     * Created with socket(addr_family, SOCK_DGRAM, IPPROTO_UDP).
     * 
     * @param addr_family Address family of upstream resolver (AF_INET or AF_INET6)
     * @return true if socket created successfully, false on error
     */
    bool initialize_upstream(sa_family_t addr_family);
    
    /**
     * @brief Get listening socket file descriptor
     * @return Socket FD or -1 if not initialized
     */
    int get_listen_fd() const { return listen_socket; }
    
    /**
     * @brief Get upstream socket file descriptor
     * @return Socket FD or -1 if not initialized
     */
    int get_upstream_fd() const { return upstream_socket; }
    
    /**
     * @brief Check if sockets are initialized
     * @return true if both sockets are valid
     */
    bool is_initialized() const {
        return listen_socket >= 0 && upstream_socket >= 0;
    }
    
    /**
     * @brief Manually close all sockets and reset to invalid state
     * 
     * Closes both listen_socket and upstream_socket via close() if valid (>= 0).
     * Resets descriptors to -1 after closing.
     * Automatically called by destructor, but can be invoked early for explicit cleanup.
     * Safe to call multiple times (idempotent).
     */
    void shutdown();

private:
    int listen_socket;      ///< Listening socket for client queries
    int upstream_socket;    ///< Socket for upstream resolver communication
};

#endif // SOCKET_MANAGER_H
