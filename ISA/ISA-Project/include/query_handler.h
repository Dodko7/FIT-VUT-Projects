/**
 * @file query_handler.h
 * @brief DNS query processing business logic
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-26-10
 *
 * Responsibilities:
 * - Parse and validate incoming DNS messages
 * - Check query type (only A) and class (only IN)
 * - Check domain against blocklist
 * - Build pre-constructed error responses (FORMERR, NOTIMP, REFUSED)
 * - Return decision structure with action and optional response buffer
 * - Network-agnostic: no socket I/O, only message processing
 */

#ifndef QUERY_HANDLER_H
#define QUERY_HANDLER_H

#include "common.h"
#include "filter.h"
#include "dns_protocol.h"
#include <string>
#include <cstdint>

/**
 * @brief Actions that can be taken for a DNS query
 */
enum class QueryAction {
    FORWARD,        ///< Forward query to upstream resolver
    BLOCK,          ///< Domain is blocked - send REFUSED
    ERROR_FORMAT,   ///< Malformed query - send FORMERR
    ERROR_NOTIMP    ///< Unsupported type/class - send NOTIMP
};

/**
 * @brief Decision result from query processing
 * 
 * Contains action to take and pre-built response (if error).
 * If action == FORWARD, parsed_msg is valid and response_buffer is unused.
 * Otherwise, response_buffer contains complete DNS error response.
 */
struct QueryDecision {
    QueryAction action;                       ///< Action to take
    DNSMessage parsed_msg;                    ///< Parsed message (valid only if FORWARD)
    uint8_t response_buffer[DNS_BUFFER_SIZE]; ///< Pre-built error response (512 bytes)
    size_t response_len;                      ///< Bytes written to response_buffer
    
    QueryDecision() 
        : action(QueryAction::ERROR_FORMAT)
        , parsed_msg{}
        , response_len(0) 
    {
        parsed_msg.valid = false;
    }
};

/**
 * @brief Process DNS query and determine action
 * 
 * 4-step decision flow:
 * 1. Parse DNS message
 *    - Success: continue to step 2
 *    - Failure: action=ERROR_FORMAT, build FORMERR response
 * 
 * 2. Validate QTYPE and QCLASS
 *    - Must be QTYPE=A (1) and QCLASS=IN (1)
 *    - Other types: action=ERROR_NOTIMP, build NOTIMP response
 * 
 * 3. Check domain against blocklist
 *    - Blocked: action=BLOCK, build REFUSED response
 *    - Allowed: continue to step 4
 * 
 * 4. Allow forwarding
 *    - action=FORWARD, no response built (caller forwards to upstream)
 * 
 * Pre-built responses: For ERROR_FORMAT, ERROR_NOTIMP, and BLOCK actions,
 * decision.response_buffer contains complete DNS response ready to send.
 * 
 * @param query_buffer Raw DNS query bytes
 * @param query_len Buffer length
 * @param filter Domain blocklist
 * @param client_addr_str Client address for logging
 * @return QueryDecision with action and optional pre-built response
 */
QueryDecision handle_query(
    const uint8_t* query_buffer,
    size_t query_len,
    const DomainFilter& filter,
    const std::string& client_addr_str
);

/**
 * @brief Convert QueryAction to string for logging
 * 
 * Returns: "FORWARD", "BLOCK", "ERROR_FORMAT", "ERROR_NOTIMP", or "UNKNOWN"
 * 
 * @param action Action enum value
 * @return String name
 */
const char* query_action_to_string(QueryAction action);

#endif // QUERY_HANDLER_H
