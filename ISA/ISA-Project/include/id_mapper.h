/**
 * @file id_mapper.h
 * @brief ID mapping utility to avoid ID collisions when forwarding queries
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-17-10
 * 
 * Responsibilities:
 * - Allocate unique forwarded IDs (1-65535) to prevent client ID collisions
 * - Map forwarded IDs to original client information (address, ID, timestamp)
 * - Thread-safe operations using mutex protection
 * - Random ID allocation with linear scan fallback
 * - Time-based cleanup of stale mappings to prevent memory leaks
 */

#ifndef ID_MAPPER_H
#define ID_MAPPER_H

#include "common.h"
#include <cstdint>
#include <ctime>
#include <netinet/in.h>
#include <unordered_map>

/**
 * @brief Mapping entry storing client information for forwarded query
 * 
 * Stores all data needed to route response back to original client.
 */
struct ForwardEntry {
    sockaddr_storage client_addr;  // Client socket address (IPv4/IPv6)
    socklen_t client_addr_len;     // Size of client_addr structure
    uint16_t orig_id;              // Original client DNS transaction ID
    time_t timestamp;              // Entry creation time (for timeout cleanup)
    size_t query_len;              // Original query buffer size
};

/**
 * @brief Initialize ID mapper with random seed
 * 
 * Sets up std::mt19937 random number generator using std::random_device.
 * Call once at startup before using id_mapper_allocate().
 */
void id_mapper_init();

/**
 * @brief Allocate unique forwarded ID and store mapping
 * 
 * Algorithm:
 * 1. Try 20 random IDs (0x0001-0xFFFF, skip 0) - fast path
 * 2. If all collide, linear scan 1-65535 - slow fallback
 * 3. Store entry with current timestamp
 * 
 * Thread-safe: uses mutex lock.
 * 
 * @param entry Client information to store (orig_id, client_addr, etc.)
 * @return Allocated forwarded ID (1-65535), or 0 if all IDs in use
 */
uint16_t id_mapper_allocate(const ForwardEntry& entry);

/**
 * @brief Retrieve and remove mapping for forwarded ID
 * 
 * Lookup is O(1) using std::unordered_map.
 * Entry is removed from table after retrieval (pop operation).
 * Thread-safe: uses mutex lock.
 * 
 * @param forwarded_id ID from upstream response
 * @param out_entry Output: retrieved mapping entry
 * @return true if found and removed, false if not found
 */
bool id_mapper_pop(uint16_t forwarded_id, ForwardEntry& out_entry);

/**
 * @brief Remove mappings older than timeout threshold
 * 
 * Iterates all entries, compares current time - entry.timestamp > timeout_seconds.
 * Erases expired entries during iteration.
 * Thread-safe: uses mutex lock.
 * 
 * @param timeout_seconds Age threshold in seconds (default: ID_MAPPING_TIMEOUT_SEC)
 */
void id_mapper_cleanup(int timeout_seconds = ID_MAPPING_TIMEOUT_SEC);

/**
 * @brief Clear all mappings and free resources
 * 
 * Calls active_mappings.clear().
 * Thread-safe: uses mutex lock.
 */
void id_mapper_shutdown();

#endif // ID_MAPPER_H
