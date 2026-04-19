/**
 * @file domain_utils.cpp
 * @brief Implementation of domain validation and normalization
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 */

#include "../include/domain_utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

// ============================================================================
// Normalization Functions
// ============================================================================

std::string DomainUtils::strip_url_scheme(const std::string& input) {
    std::string result = input;
    
    // Remove common URL schemes
    if (result.find(URL_SCHEME_HTTP) == 0) {
        result = result.substr(URL_SCHEME_HTTP.length());
    }
    else if (result.find(URL_SCHEME_HTTPS) == 0) {
        result = result.substr(URL_SCHEME_HTTPS.length());
    }
    else if (result.find(URL_SCHEME_FTP) == 0) {
        result = result.substr(URL_SCHEME_FTP.length());
    }
    
    // Remove path component (everything after first '/')
    size_t path_pos = result.find('/');
    if (path_pos != std::string::npos) {
        result = result.substr(0, path_pos);
    }
    
    // Remove query parameters (everything after '?')
    size_t query_pos = result.find('?');
    if (query_pos != std::string::npos) {
        result = result.substr(0, query_pos);
    }
    
    // Remove fragment (everything after '#')
    size_t fragment_pos = result.find('#');
    if (fragment_pos != std::string::npos) {
        result = result.substr(0, fragment_pos);
    }
    
    return result;
}

std::string DomainUtils::normalize_domain(const std::string& domain) {
    // Find first non-whitespace character
    size_t start = domain.find_first_not_of(WHITESPACE_CHARS);
    
    // Return empty string if domain is all whitespace
    if (start == std::string::npos) {
        return ""; 
    }
    
    // Find last non-whitespace character
    size_t end = domain.find_last_not_of(WHITESPACE_CHARS);
    
    // Extract substring without leading/trailing whitespace
    std::string normalized = domain.substr(start, end - start + 1);
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), 
                            [](unsigned char c) { return std::tolower(c); });
    
    // Remove trailing dot
    if (!normalized.empty() && normalized.back() == DOMAIN_SEPARATOR) {
        normalized.pop_back();
    }
    
    return normalized;
}

// ============================================================================
// Validation Functions
// ============================================================================

bool DomainUtils::validate_domain(const std::string& domain) {
    // Domain name cannot be empty or too long
    if (domain.empty() || domain.length() > DOMAIN_MAX_LENGTH) {
        return false;
    }
    
    // Reject invalid dot placement
    if (domain.front() == DOMAIN_SEPARATOR || 
        domain.back() == DOMAIN_SEPARATOR || 
        domain.find("..") != std::string::npos) {
        return false;
    }
    
    // Validate all characters
    for (char c : domain) {
        if (!is_valid_dns_char(c)) {
            return false;
        }
    }
    

    // Split domain into labels and validate each one
    std::istringstream label_stream(domain);
    std::string label;
    
    while (std::getline(label_stream, label, DOMAIN_SEPARATOR)) {
        // Label cannot be empty or exceed maximum length
        if (label.empty() || label.length() > DOMAIN_LABEL_MAX) {
            return false;
        }
        
        // Label cannot start or end with hyphen
        if (label.front() == LABEL_HYPHEN || label.back() == LABEL_HYPHEN) {
            return false;
        }
        
        // Label cannot contain consecutive hyphens
        if (label.find("--") != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool DomainUtils::is_valid_dns_char(char c) {
    // Allow alphanumeric characters, hyphens, and dots
    return std::isalnum(static_cast<unsigned char>(c)) || 
            c == LABEL_HYPHEN || 
            c == DOMAIN_SEPARATOR;
}

bool DomainUtils::is_valid_ipv4(const std::string& ip) {
    // Empty string is invalid
    if (ip.empty()) return false;

    // Use string stream to split by dots
    std::istringstream stream(ip);

    // Each octet string
    std::string octet;

    // Count of octets
    int count = 0;
    
    // Split by dots and validate each octet
    while (std::getline(stream, octet, DOMAIN_SEPARATOR)) {
        if (++count > 4) return false;  // Too many octets
        
        // Empty or too long
        if (octet.empty() || octet.length() > 3) return false;
        
        // No leading zeros except "0" itself
        if (octet.length() > 1 && octet[0] == '0') return false;
        
        // Must be all digits
        for (char c : octet) {
            if (!std::isdigit(static_cast<unsigned char>(c))) return false;
        }
        
        // Range check: 0-255
        int value = std::stoi(octet);
        if (value < 0 || value > 255) return false;
    }
    
    return count == 4;  // Must have exactly 4 octets
}

bool DomainUtils::is_valid_ipv6(const std::string& ip) {
    // Empty string is invalid
    if (ip.empty()) return false;
    
    // Count colons and check for double colon
    size_t colon_count = 0;
    size_t double_colon_pos = ip.find("::");
    
    // Count colons and check for valid hex digits
    for (char c : ip) {
        if (c == ':') {
            colon_count++;
        } else if (!std::isxdigit(static_cast<unsigned char>(c))) {
            return false;  // Invalid character
        }
    }
    
    // Check for presence of "::"
    bool has_double_colon = (double_colon_pos != std::string::npos);
    bool only_one_double = (has_double_colon && 
                            ip.find("::", double_colon_pos + 2) == std::string::npos);
    
    // Valid IPv6 must have between 2 and 7 colons, with at most one "::"
    return colon_count >= 2 && colon_count <= 7 && 
            (!has_double_colon || only_one_double);
}

// ============================================================================
// Matching Functions
// ============================================================================

bool DomainUtils::is_subdomain(const std::string& query_domain, 
                                const std::string& filter_domain) {
    // Fast path: exact match
    if (query_domain == filter_domain) {
        return true;
    }
    
    // Subdomain must be longer than parent
    if (query_domain.length() <= filter_domain.length()) {
        return false;
    }
    
    // Check if query ends with "." + filter_domain
    size_t pos = query_domain.length() - filter_domain.length();
    
    // Must have at least one character before for the dot
    if (pos == 0) {
        return false;
    }
    
    // Verify dot separator and matching suffix
    return query_domain[pos - 1] == DOMAIN_SEPARATOR && 
            query_domain.substr(pos) == filter_domain;
}