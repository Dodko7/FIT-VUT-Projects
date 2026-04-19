/**
 * @file query_handler.cpp
 * @brief Implementation of DNS query processing logic
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-26-10
 */

#include "../include/query_handler.h"
#include "../include/dns_protocol.h"
#include "../include/dns_response.h"
#include "../include/logger.h"

// ============================================================================
// Query Processing Business Logic
// ============================================================================

QueryDecision handle_query(const uint8_t* query_buffer,
                            size_t query_len,
                            const DomainFilter& filter,
                            const std::string& client_addr_str) {
    // Initialize decision structure                            
    QueryDecision decision;
    
    // ========================================================================
    // Step 1: Parse DNS Message
    // ========================================================================
    LOG_INFO_F("Processing query from %s (%zu bytes)", client_addr_str.c_str(), query_len);
    
    // Parse the DNS message from the buffer
    decision.parsed_msg = parse_dns_message(query_buffer, query_len);
    
    // Check if parsing was successful and send FORMERR if not
    if (!decision.parsed_msg.valid) {
        // Parsing failed - build FORMERR response
        LOG_WARNING_F("Failed to parse DNS message from %s", client_addr_str.c_str());

        //> Set action to FORMERR
        decision.action = QueryAction::ERROR_FORMAT;

        // Build FORMERR response
        int response_length = build_formerr_response(
            query_buffer, query_len,
            decision.response_buffer, sizeof(decision.response_buffer)
        );
        
        // Check if response was built successfully
        if (response_length > 0) {
            // Set response length
            decision.response_len = static_cast<size_t>(response_length);
        } else {
            LOG_ERROR("Failed to build FORMERR response");
            // Set response length to 0 to indicate failure
            decision.response_len = 0;
        }
        
        return decision;
    }
    
    // ========================================================================
    // Step 2: Validate Query Type and Class
    // ========================================================================

    // Support only A records (type=1) and IN class (class=1)
    if (decision.parsed_msg.question.qtype != DNS_QTYPE_A || 
        decision.parsed_msg.question.qclass != DNS_QCLASS_IN) {
        
        LOG_INFO_F("Unsupported query: type=%u class=%u domain=%s",
                    decision.parsed_msg.question.qtype,
                    decision.parsed_msg.question.qclass,
                    decision.parsed_msg.question.qname.c_str());
        
        //> Set action to NOTIMP
        decision.action = QueryAction::ERROR_NOTIMP;
        
        // Build NOTIMP response
        int response_length = build_notimp_response(
            query_buffer, query_len,
            decision.response_buffer, sizeof(decision.response_buffer)
        );
        
        // Check if response was built successfully
        if (response_length > 0) {
            // Set response length
            decision.response_len = static_cast<size_t>(response_length);
        } else {
            LOG_ERROR("Failed to build NOTIMP response");
            // Set response length to 0 to indicate failure
            decision.response_len = 0;
        }
        
        return decision;
    }
    
    // ========================================================================
    // Step 3: Check Domain Filter (Blocklist)
    // ========================================================================
    
    // Extract queried domain name from parsed message
    const std::string& qname = decision.parsed_msg.question.qname;
    
    // Check if domain is blocked
    if (filter.is_blocked(qname)) {
        LOG_INFO_F("Blocking query for domain: %s", qname.c_str());
        
        //> Set action to BLOCK
        decision.action = QueryAction::BLOCK;
        
        // Build REFUSED response
        int response_length = build_refused_response(
            query_buffer, query_len,
            decision.response_buffer, sizeof(decision.response_buffer)
        );
        
        // Check if response was built successfully
        if (response_length > 0) {
            // Set response length
            decision.response_len = static_cast<size_t>(response_length);
        } else {
            LOG_ERROR("Failed to build REFUSED response");
            // Set response length to 0 to indicate failure
            decision.response_len = 0;
        }
        
        return decision;
    }
    
    // ========================================================================
    // Step 4: Query is Valid and Allowed - Forward to Upstream
    // ========================================================================
    LOG_INFO_F("Allowing query for domain: %s", qname.c_str());
    
    //> Set action to FORWARD
    decision.action = QueryAction::FORWARD;
    
    // No error response needed
    decision.response_len = 0;  // No response built yet
    
    return decision;
}

// ============================================================================
// Utility Functions
// ============================================================================

const char* query_action_to_string(QueryAction action) {
    switch (action) {
        case QueryAction::FORWARD:      return "FORWARD";
        case QueryAction::BLOCK:        return "BLOCK";
        case QueryAction::ERROR_FORMAT: return "ERROR_FORMAT";
        case QueryAction::ERROR_NOTIMP: return "ERROR_NOTIMP";
        default:                        return "UNKNOWN";
    }
}
