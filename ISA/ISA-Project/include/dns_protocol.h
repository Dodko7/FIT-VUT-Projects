/**
 * @file dns_protocol.h
 * @brief DNS message parsing and protocol structures
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-18-10
 *
 * Responsibilities:
 * - DNS header structure definition with network byte order handling
 * - Parse DNS messages with ntohs()/htons() byte order conversion
 * - Validate message format (length, question count)
 * - Extract question section fields (QNAME, QTYPE, QCLASS)
 * - Utility functions for DNS type/flag string conversion
 */

#ifndef DNS_PROTOCOL_H
#define DNS_PROTOCOL_H

#include "common.h"
#include <string>
#include <cstdint>

// ============================================================================
// DNS Message Structures
// ============================================================================

/**
 * @brief DNS header structure (12 bytes, RFC1035 section 4.1.1)
 * 
 * All fields stored in network byte order (big-endian).
 * Use ntohs() when reading, htons() when writing.
 * __attribute__((packed)) prevents compiler padding.
 */
struct DNSHeader {
    uint16_t id;        // Transaction ID
    uint16_t flags;     // QR, Opcode, AA, TC, RD, RA, Z, RCODE
    uint16_t qdcount;   // Question count
    uint16_t ancount;   // Answer RR count
    uint16_t nscount;   // Authority RR count
    uint16_t arcount;   // Additional RR count
} __attribute__((packed));

/**
 * @brief Parsed question section data
 */
struct DNSQuestion {
    std::string qname;  // Domain name (normalized: lowercase, no trailing dot)
    uint16_t qtype;     // Query type (host byte order)
    uint16_t qclass;    // Query class (host byte order)
};

/**
 * @brief Fully parsed DNS message with converted fields
 * 
 * All numeric fields converted to host byte order for easy use.
 * The .valid flag indicates successful parsing.
 */
struct DNSMessage {
    uint16_t id;            // Transaction ID (host order)
    uint16_t flags;         // Flags field (host order)
    uint16_t qdcount;       // Question count
    uint16_t ancount;       // Answer count
    uint16_t nscount;       // Authority count
    uint16_t arcount;       // Additional count
    DNSQuestion question;   // First question (only one supported)
    bool valid;             // true if parsing succeeded
};

// ============================================================================
// DNS Message Parsing Functions
// ============================================================================

/**
 * @brief Parse complete DNS message from raw buffer
 * 
 * Steps:
 * 1. Validate message length (min: DNS_HEADER_SIZE, max: DNS_BUFFER_SIZE)
 * 2. Overlay DNSHeader struct and convert all fields to host byte order
 * 3. Check QDCOUNT == 1 (only single-question messages supported)
 * 4. Extract QNAME using extract_qname() (handles compression)
 * 5. Extract QTYPE and QCLASS (2 bytes each, after QNAME)
 * 6. Set msg.valid = true if all steps succeed
 * 
 * @param query_buffer Raw DNS message bytes
 * @param len Buffer length
 * @return DNSMessage structure (check .valid field before use)
 */
DNSMessage parse_dns_message(const uint8_t* query_buffer, size_t len);

/**
 * @brief Validate DNS message length
 * 
 * Checks:
 * - Minimum: DNS_HEADER_SIZE (12 bytes)
 * - Maximum: DNS_BUFFER_SIZE (512 bytes for UDP)
 * 
 * @param len Message length in bytes
 * @return true if length is valid
 */
bool is_valid_dns_message(size_t len);

/**
 * @brief Check if DNS header has exactly one question
 * 
 * Converts header->qdcount from network to host order and checks == 1.
 * Messages with multiple questions are not supported.
 * 
 * @param header DNS header pointer (fields in network byte order)
 * @return true if QDCOUNT == 1
 */
bool has_single_question(const DNSHeader* header);

/**
 * @brief Extract QTYPE and QCLASS fields from question section
 * 
 * Reads 4 bytes starting at qname_end offset:
 * - Bytes 0-1: QTYPE (network order) -> convert to host order
 * - Bytes 2-3: QCLASS (network order) -> convert to host order
 * 
 * @param query_buffer DNS message buffer
 * @param qname_end Offset immediately after QNAME
 * @param len Total buffer length
 * @param qtype Output: query type (host order)
 * @param qclass Output: query class (host order)
 * @return true if successfully extracted
 */
bool extract_question_fields(const uint8_t* query_buffer, size_t qname_end, size_t len,
                            uint16_t& qtype, uint16_t& qclass);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Convert DNS flags field to human-readable string
 * 
 * Extracts and formats: QR, AA, RD, RA flags and RCODE value.
 * Example output: "QR RD RCODE=0"
 * 
 * @param flags Flags field (host byte order)
 * @return String representation
 */
std::string dns_flags_to_string(uint16_t flags);

/**
 * @brief Convert QTYPE value to string name
 * 
 * Known types: A, AAAA, MX, NS
 * Unknown types: "TYPE<number>"
 * 
 * @param qtype Query type value
 * @return String name
 */
std::string dns_qtype_to_string(uint16_t qtype);

#endif // DNS_PROTOCOL_H