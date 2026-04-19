/**
 * @file id_mapper.cpp
 * @brief Implementation of simple ID mapping table
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-17-10
 */

#include "../include/id_mapper.h"
#include "../include/logger.h"
#include <mutex>
#include <random>
#include <algorithm>

// Global mapping table for forwarded DNS query IDs
static std::unordered_map<uint16_t, ForwardEntry> active_mappings;
// Mutex to protect the mapping table from concurrent access
static std::mutex mappings_mutex;
// Random number generator for ID allocation
static std::mt19937 id_generator;

// Initialize the ID mapper with a random seed
void id_mapper_init() {
    std::random_device seed_device;
    id_generator.seed(seed_device());
}

// Allocate a new forwarded ID and store the mapping entry
uint16_t id_mapper_allocate(const ForwardEntry& entry) {
    std::lock_guard<std::mutex> lock(mappings_mutex);

    // Try random IDs first to reduce collisions
    for (int attempts = 0; attempts < 20; ++attempts) {
        // Generate a random 16-bit ID
        uint16_t new_id = static_cast<uint16_t>(id_generator() & 0xFFFFu);

        // Avoid ID 0 for clarity
        if (new_id == 0) continue;

        // Check if this ID is already in use
        if (active_mappings.find(new_id) == active_mappings.end()) {
            ForwardEntry new_entry = entry;
            new_entry.timestamp = std::time(nullptr);
            active_mappings.emplace(new_id, new_entry);
            return new_id;
        }
    }

    // Fallback: linear scan if random attempts fail
    for (uint32_t id = 1; id <= 0xFFFF; ++id) {
        // Generate candidate ID
        uint16_t candidate = static_cast<uint16_t>(id);

        // Check if this ID is already in use
        if (active_mappings.find(candidate) == active_mappings.end()) {
            ForwardEntry new_entry = entry;
            new_entry.timestamp = std::time(nullptr);
            active_mappings.emplace(candidate, new_entry);
            return candidate;
        }
    }

    LOG_ERROR("ID mapper: no available IDs to allocate");
    
    // No available IDs (all 65535 are in use)
    return 0;
}

// Retrieve and remove a mapping entry by forwarded ID
bool id_mapper_pop(uint16_t forwarded_id, ForwardEntry& result) {
    // Lock the mapping table to prevent concurrent access
    std::lock_guard<std::mutex> lock(mappings_mutex);

    // Find the mapping entry
    auto map_iterator = active_mappings.find(forwarded_id);

    // If not found, return false
    if (map_iterator == active_mappings.end()) return false;

    // Copy the found entry to output parameter
    result = map_iterator->second;

    // Remove the entry from the mapping table
    active_mappings.erase(map_iterator);

    return true;
}

// Remove expired entries from the mapping table
void id_mapper_cleanup(int timeout_seconds) {
    // Lock the mapping table to prevent concurrent access during cleanup
    std::lock_guard<std::mutex> lock(mappings_mutex);
    
    // Get current timestamp for comparison with entry timestamps
    time_t now = std::time(nullptr);
    
    // Iterate through all active mappings and remove expired entries
    for (auto map_iterator = active_mappings.begin(); map_iterator != active_mappings.end(); ) {
        // Check if entry has exceeded timeout threshold
        if (now - map_iterator->second.timestamp > timeout_seconds) {
            // Remove expired entry and advance iterator
            map_iterator = active_mappings.erase(map_iterator);
        } else {
            // Keep entry and move to next
            ++map_iterator;
        }
    }
}

// Clear all entries from the mapping table
void id_mapper_shutdown() {
    std::lock_guard<std::mutex> lock(mappings_mutex);
    active_mappings.clear();
}
