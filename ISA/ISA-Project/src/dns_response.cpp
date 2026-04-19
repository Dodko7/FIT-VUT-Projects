/**
 * @file dns_response.cpp
 * @brief Implementation of DNS response construction
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-26-10
 */

#include "../include/dns_response.h"
#include "../include/dns_compression.h"
#include "../include/logger.h"
#include <cstring>
#include <arpa/inet.h>

// ============================================================================
// Internal Helper Functions
// ============================================================================

/**
 * @brief Copy question section from request to response
 * 
 * Walks through the QNAME (handling compression pointers) and copies
 * the entire question section (QNAME + QTYPE + QCLASS) to output buffer.
 * 
 * @param request_buffer Request buffer
 * @param request_length Request length
 * @param output_buffer Output buffer
 * @param output_length Output buffer size
 * @param question_end_offset Output: offset after question section
 * @return true if successful, false on error
 */
static bool copy_question_section(const uint8_t* request_buffer, size_t request_length,
                                uint8_t* output_buffer, size_t output_length, size_t& question_end_offset) {
    // Copy entire question section (QNAME + QTYPE + QCLASS)
    if (request_length < DNS_HEADER_SIZE) return false;

    // Parse QNAME length by walking from DNS_HEADER_SIZE until zero label or pointer
    size_t offset = DNS_HEADER_SIZE;
    size_t walked = 0;
    
    while (offset < request_length && walked < request_length) {
        uint8_t label_length = request_buffer[offset];
        
        // If compression pointer, two bytes and end of question name in request
        if (is_compression_pointer(label_length)) {
            if (offset + 1 >= request_length) return false;
            offset += 2;
            break;
        }
        
        offset++;
        if (label_length == 0) break;
        offset += label_length;
        walked += label_length + 1;
    }

    // Now need 4 bytes for QTYPE and QCLASS
    if (offset + 4 > request_length) return false;

    size_t question_section_length = offset + 4 - DNS_HEADER_SIZE;
    if (DNS_HEADER_SIZE + question_section_length > output_length) return false;

    // Copy question into output_buffer after header (we'll later write header)
    std::memcpy(output_buffer + DNS_HEADER_SIZE, request_buffer + DNS_HEADER_SIZE, question_section_length);
    question_end_offset = DNS_HEADER_SIZE + question_section_length;
    return true;
}

/**
 * @brief Build a DNS response with specified RCODE
 * 
 * Generic response builder that:
 * - Preserves original ID
 * - Sets response flags (QR=1)
 * - Preserves RD flag from request
 * - Copies question section
 * - Sets specified RCODE
 * 
 * @param request_buffer Request buffer
 * @param request_length Request length
 * @param output_buffer Output buffer
 * @param output_length Output buffer size
 * @param rcode Response code to set
 * @return Number of bytes written, or -1 on error
 */
static int build_response_with_rcode(const uint8_t* request_buffer, size_t request_length,
                                    uint8_t* output_buffer, size_t output_length, uint8_t rcode) {
    // Validate input buffer
    if (request_length < DNS_HEADER_SIZE) {
        LOG_WARNING("Cannot build response: request too short");
        return -1;
    }

    // Ensure output buffer large enough for header + question
    if (output_length < DNS_HEADER_SIZE) {
        LOG_WARNING("Cannot build response: output buffer too small");
        return -1;
    }

    // Zero-initialize output buffer
    std::memset(output_buffer, 0, output_length);

    // Copy original ID from request
    const uint16_t* request_id_ptr = reinterpret_cast<const uint16_t*>(request_buffer);
    uint16_t* output_id_ptr = reinterpret_cast<uint16_t*>(output_buffer);
    *output_id_ptr = *request_id_ptr;  // Already in network byte order

    // Build flags field
    const uint16_t* request_flags_ptr = reinterpret_cast<const uint16_t*>(request_buffer + 2);
    uint16_t request_flags = ntohs(*request_flags_ptr);
    
    uint16_t flags = 0;
    flags |= DNS_FLAG_QR;  // Set QR=1 (this is a response)
    
    // Preserve RD (Recursion Desired) from request
    if (request_flags & DNS_FLAG_RD) {
        flags |= DNS_FLAG_RD;
    }
    
    // Set RCODE in low 4 bits
    flags |= (rcode & 0x0F);
    
    // Write flags to output
    uint16_t* output_flags_ptr = reinterpret_cast<uint16_t*>(output_buffer + 2);
    *output_flags_ptr = htons(flags);

    // Set counts: QDCOUNT=1, others=0 (error responses have no answers)
    uint16_t* qdcount_ptr = reinterpret_cast<uint16_t*>(output_buffer + 4);
    *qdcount_ptr = htons(1);
    
    uint16_t* ancount_ptr = reinterpret_cast<uint16_t*>(output_buffer + 6);
    *ancount_ptr = htons(0);
    
    uint16_t* nscount_ptr = reinterpret_cast<uint16_t*>(output_buffer + 8);
    *nscount_ptr = htons(0);
    
    uint16_t* arcount_ptr = reinterpret_cast<uint16_t*>(output_buffer + 10);
    *arcount_ptr = htons(0);

    // Copy question section from request
    size_t question_end_offset = 0;
    if (!copy_question_section(request_buffer, request_length, output_buffer, output_length, question_end_offset)) {
        LOG_WARNING("Failed to copy question section to response - sending header-only response");
        
        // For severely malformed queries, send a minimal header-only response with QDCOUNT=0
        uint16_t* qdcount_ptr = reinterpret_cast<uint16_t*>(output_buffer + 4);
        *qdcount_ptr = htons(0);  // Override QDCOUNT to 0
        
        return DNS_HEADER_SIZE;  // Return just the header
    }

    // Return total length of response
    return static_cast<int>(question_end_offset);
}

// ============================================================================
// Public API - Response Builders
// ============================================================================

int build_formerr_response(const uint8_t* request_buffer, size_t request_length,
                            uint8_t* output_buffer, size_t output_length) {
    LOG_INFO("Building FORMERR response for malformed query");
    return build_response_with_rcode(request_buffer, request_length, output_buffer, output_length, DNS_RCODE_FORMERR);
}

int build_notimp_response(const uint8_t* request_buffer, size_t request_length,
                            uint8_t* output_buffer, size_t output_length) {
    LOG_INFO("Building NOTIMP response for unsupported query type");
    return build_response_with_rcode(request_buffer, request_length, output_buffer, output_length, DNS_RCODE_NOTIMP);
}

int build_refused_response(const uint8_t* request_buffer, size_t request_length,
                            uint8_t* output_buffer, size_t output_length) {
    LOG_INFO("Building REFUSED response for blocked domain");
    return build_response_with_rcode(request_buffer, request_length, output_buffer, output_length, DNS_RCODE_REFUSED);
}
