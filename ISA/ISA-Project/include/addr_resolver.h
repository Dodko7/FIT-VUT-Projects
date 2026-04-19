/**
 * @file addr_resolver.h
 * @brief Address resolution utilities for DNS server
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 * 
 * Responsibilities:
 * - Resolve upstream server hostname/IP to sockaddr structure using getaddrinfo()
 * - Support both IPv4 and IPv6 addresses for upstream DNS server
 * - Store resolved address information in configuration structure
 */

#ifndef ADDR_RESOLVER_H
#define ADDR_RESOLVER_H

#include "arg_parser.h"

/**
 * @brief Resolves server hostname/IP to sockaddr structure
 * @param config Configuration structure containing server name and storage for resolved address
 * @return true if resolution successful, false otherwise
 */
bool resolve_server_address(Config& config);

#endif // ADDR_RESOLVER_H