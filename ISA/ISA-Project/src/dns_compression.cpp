/**
 * @file dns_compression.cpp
 * @brief Implementation of DNS name compression utilities
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-26-10
 */

#include "../include/dns_compression.h"
#include "../include/logger.h"
#include "../include/domain_utils.h"
#include <cstring>

// ============================================================================
// DNS Name Parsing with Compression Support
// ============================================================================

std::string parse_compressed_name(const uint8_t* buffer, size_t len, size_t& offset,
                                bool* visited_offsets) {
    // Resulting domain name
    std::string domain_name;
    
    // Track if we have jumped via a compression pointer
    bool jumped = false;

    // Save original offset in case of jumps
    size_t original_offset = offset;
    
    // Parse labels until we hit the end (zero-length label) or error
    while (offset < len) {
        // Read label length byte
        uint8_t label_len = buffer[offset];
        
        // Check for compression pointer
        if (is_compression_pointer(label_len)) {
            if (offset + 1 >= len) {
                LOG_WARNING("DNS name compression: truncated pointer");
                return "";
            }
            
            // Extract pointer offset
            uint16_t pointer_offset = get_compression_offset(buffer[offset], buffer[offset + 1]);
            
            // Check if the pointer offset is valid (must be within message bounds)
            // Note: We allow both backward and forward pointers per RFC1035 robustness
            if (pointer_offset >= len) {
                LOG_WARNING_F("DNS name compression: invalid offset %u (>= message length %zu)", pointer_offset, len);
                return "";
            }
            
            // Check for pointer loops
            if (visited_offsets[pointer_offset]) {
                LOG_WARNING("DNS name compression: loop detected");
                return "";
            }

            // Mark this offset as visited
            visited_offsets[offset] = true;
            
            // Follow the pointer
            if (!jumped) {
                original_offset = offset + 2;  // Save position after pointer
                jumped = true;
            }

            // Update offset to pointer target
            offset = pointer_offset;
            continue;
        }
        
        // Check for end of name (zero-length label)
        if (label_len == 0) {
            offset++;
            break;
        }
        
        // Validate label length
        if (label_len > DOMAIN_LABEL_MAX) {
            LOG_WARNING_F("DNS label too long: %u bytes", label_len);
            return "";
        }
        
        // Check if we have enough bytes for this label
        if (offset + 1 + label_len > len) {
            LOG_WARNING("DNS name: truncated label");
            return "";
        }
        
        // Extract label
        std::string label(reinterpret_cast<const char*>(&buffer[offset + 1]), label_len);
        
        // Validate label characters
        if (!is_valid_dns_label(label)) {
            LOG_WARNING_F("DNS name: invalid label '%s'", label.c_str());
            return "";
        }
        
        // Add to domain_name
        if (!domain_name.empty()) {
            domain_name += ".";
        }
        domain_name += label;
        
        offset += 1 + label_len;
    }
    
    // If we jumped due to compression, restore original position
    if (jumped) {
        offset = original_offset;
    }
    
    return domain_name;
}

std::string extract_qname(const uint8_t* buffer, size_t len, size_t& offset) {
    // Track visited offsets to detect compression loops
    bool visited_offsets[DNS_BUFFER_SIZE] = {false};
    
    // Start parsing from given offset
    std::string qname = parse_compressed_name(buffer, len, offset, visited_offsets);
    
    // Check if parsing was successful
    if (qname.empty()) {
        LOG_WARNING("Failed to extract QNAME from DNS message");
        return "";
    }
    
    // Normalize domain name (lowercase, no trailing dot)
    return DomainUtils::normalize_domain(qname);
}

// ============================================================================
// DNS Label Validation
// ============================================================================

bool is_valid_dns_label(const std::string& label) {
    // Check label length
    if (label.empty() || label.length() > DOMAIN_LABEL_MAX) {
        return false;
    }
    
    // Check each character in label
    for (char c : label) {
        if (!DomainUtils::is_valid_dns_char(c) || c == '.') {
            return false;
        }
    }
    
    // Labels cannot start or end with hyphen
    if (label.front() == '-' || label.back() == '-') {
        return false;
    }
    
    return true;
}
