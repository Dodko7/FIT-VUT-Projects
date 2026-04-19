/**
 * @file dns_response.h
 * @brief DNS response message construction for error handling and blocked domains
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-20-10
 *
 * Responsibilities:
 * - Build DNS error responses (FORMERR, NOTIMP, REFUSED)
 * - Copy original question section from request to response
 * - Preserve original transaction ID and RD flag
 * - Set appropriate response flags (QR) and RCODE values
 * - Set all answer/authority/additional counts to zero (error responses)
 */

#ifndef DNS_RESPONSE_H
#define DNS_RESPONSE_H

#include "common.h"
#include <cstdint>

/**
 * @brief Build FORMERR (RCODE=1) response for malformed queries
 * 
 * Response construction:
 * 1. Copy original transaction ID from request
 * 2. Set flags: QR=1 (response), preserve RD flag, RCODE=1
 * 3. Set counts: QDCOUNT=1, ANCOUNT=0, NSCOUNT=0, ARCOUNT=0
 * 4. Copy entire question section from request (QNAME + QTYPE + QCLASS)
 * 
 * Use cases:
 * - Truncated messages (too short for header or question)
 * - Invalid QNAME encoding or compression pointers
 * 
 * @param request_buffer Original request bytes
 * @param request_length Request buffer length
 * @param output_buffer Buffer for response (recommend 512 bytes)
 * @param output_length Size of output buffer
 * @return Bytes written to output_buffer, or -1 on error
 */
int build_formerr_response(const uint8_t* request_buffer, size_t request_length,
                            uint8_t* output_buffer, size_t output_length);

/**
 * @brief Build NOTIMP (RCODE=4) response for unsupported query types
 * 
 * Response construction:
 * 1. Copy original transaction ID from request
 * 2. Set flags: QR=1 (response), preserve RD flag, RCODE=4
 * 3. Set counts: QDCOUNT=1, ANCOUNT=0, NSCOUNT=0, ARCOUNT=0
 * 4. Copy entire question section from request
 * 
 * Use cases:
 * - Non-A query types (AAAA, MX, NS, etc.)
 * - Non-IN query classes
 * 
 * @param request_buffer Original request bytes
 * @param request_length Request buffer length
 * @param output_buffer Buffer for response (recommend 512 bytes)
 * @param output_length Size of output buffer
 * @return Bytes written to output_buffer, or -1 on error
 */
int build_notimp_response(const uint8_t* request_buffer, size_t request_length,
                            uint8_t* output_buffer, size_t output_length);

/**
 * @brief Build REFUSED response - server refuses to process the request
 * 
 * Use case: Domain is on the blocklist (policy decision)
 * 
 * @param request_buffer Original request bytes
 * @param request_length Request buffer length
 * @param output_buffer Buffer for response (recommend 512 bytes)
 * @param output_length Size of output buffer
 * @return Bytes written to output_buffer, or -1 on error
 */
int build_refused_response(const uint8_t* request_buffer, size_t request_length,
                            uint8_t* output_buffer, size_t output_length);

#endif // DNS_RESPONSE_H
