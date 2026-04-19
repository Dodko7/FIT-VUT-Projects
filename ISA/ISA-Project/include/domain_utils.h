/**
 * @file domain_utils.h
 * @brief Domain validation, normalization and matching utilities
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 * 
 * Responsibilities:
 * - Domain syntax validation according to RFC standards
 * - Domain normalization (lowercase, trim whitespace, remove trailing dots)
 * - URL scheme stripping (http/https/ftp) and path removal for robustness
 * - Subdomain matching logic for DNS filter rules
 * - IPv4 and IPv6 address format validation
 * - DNS character validation for labels
 * - Static utility class with no instantiation allowed
 */

#ifndef DOMAIN_UTILS_H
#define DOMAIN_UTILS_H

#include "common.h"
#include <string>

/**
 * @brief Static utility class for all domain-related operations
 * 
 * All methods are static - no instantiation needed or allowed.
 */
class DomainUtils {
    public:
    // ========================================================================
    // Domain Normalization Methods
    // ========================================================================
    
    /**
     * @brief Strip URL scheme prefix and everything after hostname
     *
     * Removes http://, https://, ftp:// schemes, and strips path (/...),
     * query (?...), and fragment (#...) components.
     * 
     * @param input URL or domain string to process
     * @return Hostname part only
     */
    static std::string strip_url_scheme(const std::string& input);

    /**
     * @brief Normalize domain to canonical form
     *
     * Steps: trim leading/trailing whitespace, convert to lowercase,
     * remove trailing dot if present.
     *
     * @param domain Domain to normalize
     * @return Normalized domain, or empty string if input is all whitespace
     */
    static std::string normalize_domain(const std::string& domain);
    
    // ========================================================================
    // Domain Validation Methods
    // ========================================================================
    
    /**
     * @brief Validate domain name syntax according to RFC standards
     *
     * Checks:
     * - Length: 1-253 characters total, each label max 63 characters
     * - No leading, trailing, or consecutive dots
     * - Only alphanumeric, hyphen, and dot characters
     * - Labels cannot start/end with hyphen or contain "--"
     *
     * @param domain Domain to validate (case-sensitive)
     * @return true if valid RFC-compliant domain syntax
     */
    static bool validate_domain(const std::string& domain);
    
    /**
     * @brief Check if character is valid in DNS labels
     *
     * @param c Character to check
     * @return true if alphanumeric, hyphen, or dot
     */
    static bool is_valid_dns_char(char c);
    
    /**
     * @brief Validate IPv4 address format (dotted-decimal notation)
     *
     * Checks:
     * - Exactly 4 octets separated by dots
     * - Each octet is 0-255 with no leading zeros (except "0" itself)
     * - All characters are digits or dots
     *
     * @param ip IPv4 address string to validate
     * @return true if valid IPv4 format
     */
    static bool is_valid_ipv4(const std::string& ip);
    
    /**
     * @brief Validate IPv6 address format (colon-hexadecimal notation)
     *
     * Checks:
     * - 2-7 colons present
     * - Only hexadecimal digits and colons
     * - At most one "::" compression sequence
     *
     * @param ip IPv6 address string to validate
     * @return true if valid IPv6 format
     */
    static bool is_valid_ipv6(const std::string& ip);
    
    // ========================================================================
    // Domain Matching Methods
    // ========================================================================
    
    /**
     * @brief Check if query_domain matches or is subdomain of filter_domain
     *
     * Matching rules:
     * - Exact match: "example.com" matches "example.com"
     * - Subdomain: "sub.example.com" matches "example.com"
     * - Must have dot separator for subdomain (not just suffix match)
     *
     * @param query_domain Domain from DNS query (normalized)
     * @param filter_domain Domain from filter list (normalized)
     * @return true if query matches filter rule
     */
    static bool is_subdomain(const std::string& query_domain, 
                            const std::string& filter_domain);

private:
    // ========================================================================
    // Prevent instantiation - static utility class only
    // ========================================================================
    
    DomainUtils() = delete;
    DomainUtils(const DomainUtils&) = delete;
    DomainUtils& operator=(const DomainUtils&) = delete;
};

#endif // DOMAIN_UTILS_H