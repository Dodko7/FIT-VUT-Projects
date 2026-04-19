/**
 * @file filter.h
 * @brief Domain filtering logic for DNS proxy/filter
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 *
 * Responsibilities:
 * - Load and parse filter file containing blocked domain names
 * - Validate domain format according to RFC standards
 * - Normalize domains (lowercase, trim whitespace, strip URL schemes)
 * - Efficient domain blocking checks with exact and subdomain matching
 * - Hash table implementation for O(1) average case domain lookups
 * - Handle file format requirements (comments, blank lines, line endings)
 */

#ifndef FILTER_H
#define FILTER_H

#include "common.h"
#include <string>
#include <unordered_set>

/**
 * @brief Domain filter for DNS query blocking
 *
 * Uses std::unordered_set for storage:
 * - O(1) average case exact match lookups
 * - O(n) subdomain matching (iterates all entries)
 */
class DomainFilter {
private:
    std::unordered_set<std::string> blocked_domains;

public:
    /**
     * @brief Load blocked domains from filter file
     *
     * Processing steps:
     * 1. Read file line-by-line (supports \n, \r\n, \r endings)
     * 2. Skip blank lines and comments (lines starting with #)
     * 3. Strip URL schemes (http://, https://, ftp://) if present
     * 4. Normalize: trim whitespace, lowercase, remove trailing dot
     * 5. Validate domain syntax (RFC compliance)
     * 6. Skip duplicates with warning
     * 7. Insert valid domains into hash table
     *
     * Invalid domains are logged and skipped, not fatal errors.
     *
     * @param filename Path to filter file
     * @return true if at least one valid domain loaded, false on file error or empty file
     */
    bool load_filter_file(const std::string& filename);

    /**
     * @brief Check if domain should be blocked
     *
     * Algorithm:
     * 1. Normalize input domain (lowercase, trim, remove trailing dot)
     * 2. Fast path: O(1) hash lookup for exact match
     * 3. Slow path: O(n) iteration checking if domain is subdomain of any blocked entry
     *
     * Example matches:
     * - "example.com" blocks "example.com" (exact)
     * - "example.com" blocks "www.example.com" (subdomain)
     * - "example.com" blocks "sub.www.example.com" (nested subdomain)
     *
     * @param domain Domain to check (will be normalized)
     * @return true if blocked, false if allowed
     */
    bool is_blocked(const std::string& domain) const;

    /**
     * @brief Get count of loaded blocked domains
     * @return Number of domains in filter set
     */
    size_t get_domain_count() const {
        return blocked_domains.size();
    }

    /**
     * @brief Clear all loaded domains from filter
     */
    void clear() {
        blocked_domains.clear();
    }

    /**
     * @brief Check if filter has no domains loaded
     * @return true if empty, false otherwise
     */
    bool empty() const {
        return blocked_domains.empty();
    }
};

#endif // FILTER_H