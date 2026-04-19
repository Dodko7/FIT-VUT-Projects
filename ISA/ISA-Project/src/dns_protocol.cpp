/**
 * @file dns_protocol.cpp
 * @brief Implementation of DNS message parsing
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-18-10
 */

#include "../include/dns_protocol.h"
#include "../include/dns_compression.h"
#include "../include/logger.h"
#include "../include/domain_utils.h"
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib>
#include <algorithm>

// ============================================================================
// DNS Message Validation
// ============================================================================

bool is_valid_dns_message(size_t len) {
    // Check minimum DNS header size
    if (len < DNS_HEADER_SIZE) {
        LOG_WARNING_F("DNS message too short: %zu bytes (minimum %zu)", 
                    len, DNS_HEADER_SIZE);
        return false;
    }
    
    // Check maximum UDP DNS message size
    if (len > DNS_BUFFER_SIZE) {
        LOG_WARNING_F("DNS message too large: %zu bytes (maximum %zu)", 
                    len, DNS_BUFFER_SIZE);
        return false;
    }
    
    return true;
}

bool has_single_question(const DNSHeader* header) {
    // Convert to host byte order for checking
    uint16_t qdcount = ntohs(header->qdcount);

    // Accept messages with exactly one question
    return (qdcount == 1);
}

// ============================================================================
// DNS Question Section Parsing
// ============================================================================

bool extract_question_fields(const uint8_t* query_buffer, size_t qname_end, size_t len,
                            uint16_t& qtype, uint16_t& qclass) {
    // Need 4 bytes for QTYPE (2) + QCLASS (2)
    if (qname_end + 4 > len) {
        LOG_WARNING("DNS question: insufficient bytes for QTYPE/QCLASS");
        return false;
    }
    
    // Extract QTYPE and QCLASS (network byte order)
    qtype = ntohs(*reinterpret_cast<const uint16_t*>(&query_buffer[qname_end]));
    qclass = ntohs(*reinterpret_cast<const uint16_t*>(&query_buffer[qname_end + 2]));
    
    LOG_INFO_F("DNS question: QTYPE=%u, QCLASS=%u", qtype, qclass);
    
    return true;
}

// ============================================================================
// Main DNS Message Parsing
// ============================================================================

DNSMessage parse_dns_message(const uint8_t* query_buffer, size_t len) {
    // Initialize empty DNSMessage
    DNSMessage msg = {};  
    
    // Basic validation
    if (!is_valid_dns_message(len)) {
        return msg;  // msg.valid = false
    }
    
    // Overlay DNS header on raw bytes
    const DNSHeader* raw_header = reinterpret_cast<const DNSHeader*>(query_buffer);
    
    // Convert each field from network to host byte order
    msg.id = ntohs(raw_header->id);
    msg.flags = ntohs(raw_header->flags);
    msg.qdcount = ntohs(raw_header->qdcount);
    msg.ancount = ntohs(raw_header->ancount);
    msg.nscount = ntohs(raw_header->nscount);
    msg.arcount = ntohs(raw_header->arcount);
    
    LOG_INFO_F("DNS header: ID=%u, FLAGS=0x%04x, QDCOUNT=%u", 
                msg.id, msg.flags, msg.qdcount);
    
    // Verify this is a query (QR=0), not a response (QR=1)
    if (msg.flags & DNS_FLAG_QR) {
        LOG_WARNING("Rejected DNS response packet sent to listening socket (QR=1, expected query with QR=0)");
        return msg;  // msg.valid = false
    }
    
    // Check if this is a standard query with one question
    if (!has_single_question(raw_header)) {
        LOG_WARNING_F("DNS message: unsupported format (QDCOUNT=%u, ANCOUNT=%u, NSCOUNT=%u, ARCOUNT=%u)",
                        msg.qdcount, msg.ancount, msg.nscount, msg.arcount);
        return msg;  // msg.valid = false
    }

    // Offset to the question section
    size_t offset = DNS_HEADER_SIZE;
    
    // Extract domain name (QNAME) from question section and update offset
    msg.question.qname = extract_qname(query_buffer, len, offset);

    // Check if QNAME extraction was successful
    if (msg.question.qname.empty()) {
        LOG_WARNING("Failed to parse QNAME from DNS question");
        return msg;  // msg.valid = false
    }
    
    // Extract QTYPE and QCLASS from question section using updated offset
    if (!extract_question_fields(query_buffer, offset, len, 
                                msg.question.qtype, msg.question.qclass)) {
        return msg;  // msg.valid = false
    }
    
    // Validation successful
    msg.valid = true;
    
    LOG_INFO_F("DNS query parsed: domain='%s', type=%s, class=%u",
                msg.question.qname.c_str(), 
                dns_qtype_to_string(msg.question.qtype).c_str(),
                msg.question.qclass);
    
    return msg;
}

// ============================================================================
// Utility Functions
// ============================================================================

std::string dns_flags_to_string(uint16_t flags) {
    std::string result;
    
    if (flags & DNS_FLAG_QR) result += "QR ";
    if (flags & DNS_FLAG_AA) result += "AA ";
    if (flags & DNS_FLAG_RD) result += "RD ";
    if (flags & DNS_FLAG_RA) result += "RA ";
    
    uint8_t rcode = flags & 0x0F;
    result += "RCODE=" + std::to_string(rcode);
    
    return result;
}

std::string dns_qtype_to_string(uint16_t qtype) {
    switch (qtype) {
        case DNS_QTYPE_A:    return "A";
        case DNS_QTYPE_AAAA: return "AAAA";
        case DNS_QTYPE_MX:   return "MX";
        case DNS_QTYPE_NS:   return "NS";
        default:             return "TYPE" + std::to_string(qtype);
    }
}