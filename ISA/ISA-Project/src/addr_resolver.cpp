/**
 * @file addr_resolver.cpp
 * @brief Implementation of address resolution utilities
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 */

#include "../include/addr_resolver.h"
#include "../include/logger.h"
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// ============================================================================
// Helper Functions
// ============================================================================
/**
 * @brief Convert resolved address to string for logging
 * @param ai_family Address family (AF_INET or AF_INET6)
 * @param ai_addr Pointer to sockaddr structure
 * @return Address string or empty on error
 */
static std::string addr_to_string(int ai_family, const sockaddr* ai_addr) {
    // Buffer to hold string representation
    char addr_str[ADDR_STRING_MAX];

    // Pointer to result
    const char* result = nullptr;
    
    // Handle IPv4 and IPv6
    if (ai_family == AF_INET) {
        // IPv4 address
        const auto* addr_in = reinterpret_cast<const sockaddr_in*>(ai_addr);
        result = inet_ntop(AF_INET, &addr_in->sin_addr, 
                            addr_str, INET_ADDRSTRLEN);
    } 
    else if (ai_family == AF_INET6) {
        // IPv6 address
        const auto* addr_in6 = reinterpret_cast<const sockaddr_in6*>(ai_addr);
        result = inet_ntop(AF_INET6, &addr_in6->sin6_addr, 
                            addr_str, INET6_ADDRSTRLEN);
    }
    
    return (result != nullptr) ? std::string(addr_str) : std::string();
}

// ============================================================================
// Main Resolution Function
// ============================================================================
bool resolve_server_address(Config& config) {
    LOG_INFO_F("Resolving server address: %s", config.server.c_str());
    
    // Setup hints for getaddrinfo()
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;        // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;     // UDP datagram sockets
    hints.ai_protocol = IPPROTO_UDP;    // UDP protocol
    
    // Resolve the server name/IP
    struct addrinfo* result = nullptr;
    int status = getaddrinfo(config.server.c_str(), "53", &hints, &result);
    
    // Check for resolution errors
    if (status != 0) {
        LOG_ERROR_F("Failed to resolve server '%s': %s", 
                    config.server.c_str(), gai_strerror(status));
        return false;
    }
    
    // Ensure at least one result was found
    if (result == nullptr) {
        LOG_ERROR_F("No address found for server: %s", config.server.c_str());
        freeaddrinfo(result);
        return false;
    }
    
    // Use the first result
    std::memcpy(&config.server_addr, result->ai_addr, result->ai_addrlen);
    config.server_addr_len = result->ai_addrlen;
    
    // Log the resolved address
    std::string addr_str = addr_to_string(result->ai_family, result->ai_addr);
    
    // Log the resolved address
    if (!addr_str.empty()) {
        const char* family_name = (result->ai_family == AF_INET) ? "IPv4" : "IPv6";
        LOG_INFO_F("Resolved to %s address: %s", family_name, addr_str.c_str());
    } else {
        LOG_WARNING("Could not convert resolved address to string");
    }
    
    // Clean up
    freeaddrinfo(result);
    
    LOG_INFO("Server address resolution successful");
    return true;
}
