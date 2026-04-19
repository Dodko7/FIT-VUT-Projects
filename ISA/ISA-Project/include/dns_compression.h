/**
 * @file dns_compression.h
 * @brief DNS name compression and decompression utilities
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-20-10
 *
 * Responsibilities:
 * - Parse DNS names with compression pointer support (RFC1035 section 4.1.4)
 * - Detect and prevent compression pointer loops
 * - Extract QNAME from DNS messages with normalization
 * - Validate DNS label format (length, characters, hyphen placement)
 * - Safe buffer boundary checking for all pointer operations
 */

#ifndef DNS_COMPRESSION_H
#define DNS_COMPRESSION_H

#include "common.h"
#include <string>
#include <cstdint>

/**
 * @brief Check if byte is compression pointer marker
 * @param byte1 First byte to check
 * @return true if top 2 bits are 11 (0xC0 pattern)
 */
inline bool is_compression_pointer(uint8_t byte1) {
    return (byte1 & 0xC0) == 0xC0;
}

/**
 * @brief Extract 14-bit offset from compression pointer
 * @param byte1 First byte (bits 0-5 used)
 * @param byte2 Second byte (all 8 bits used)
 * @return Offset into DNS message buffer
 */
inline uint16_t get_compression_offset(uint8_t byte1, uint8_t byte2) {
    return ((byte1 & 0x3F) << 8) | byte2;
}

/**
 * @brief Parse DNS name with compression pointer following (recursive core)
 * 
 * Algorithm:
 * 1. Read label length byte at current offset
 * 2. If compression pointer (0xC0): extract offset, check for loops/validity, follow pointer
 * 3. If zero byte: end of name reached
 * 4. If label length: validate length (max 63), extract label, validate characters
 * 5. Append label to domain_name with dot separator
 * 6. Repeat until end or error
 * 7. If jumped via pointer, restore original offset after pointer (2 bytes)
 * 
 * Loop detection: uses visited_offsets array to track all visited positions.
 * 
 * @param buffer DNS message buffer
 * @param len Buffer length
 * @param offset Current offset (updated during parsing)
 * @param visited_offsets Boolean array tracking visited offsets for loop detection
 * @return Parsed domain name or empty string on error
 */
std::string parse_compressed_name(const uint8_t* buffer, size_t len, size_t& offset,
                                bool* visited_offsets);

/**
 * @brief Extract QNAME from DNS message (main entry point)
 * 
 * Steps:
 * 1. Initialize visited_offsets tracking array (DNS_BUFFER_SIZE boolean array)
 * 2. Call parse_compressed_name() to handle actual parsing
 * 3. Normalize result (lowercase, remove trailing dot)
 * 
 * This function is the public API for QNAME extraction. It handles
 * initialization and normalization, delegating parsing to parse_compressed_name().
 * 
 * @param buffer DNS message buffer
 * @param len Buffer length
 * @param offset Starting offset (updated to position after QNAME)
 * @return Normalized domain name or empty string on error
 */
std::string extract_qname(const uint8_t* buffer, size_t len, size_t& offset);

/**
 * @brief Validate DNS label format
 * 
 * Checks:
 * - Length: 1-63 characters (DOMAIN_LABEL_MAX)
 * - Characters: only alphanumeric, hyphen (no dot allowed in label)
 * - Hyphen placement: cannot be first or last character
 * 
 * @param label Label string to validate
 * @return true if valid label format
 */
bool is_valid_dns_label(const std::string& label);

#endif // DNS_COMPRESSION_H
