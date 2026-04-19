/**
 * @file common.h
 * @brief Common constants, types and definitions for DNS filter
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-16-10
 * 
 * Responsibilities:
 * - DNS protocol constants and flags
 * - Domain validation constants
 * - Application buffer sizes and timeouts
 * - Return codes and comment/whitespace handling constants
 */

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <string>

// ============================================================================
// DNS Protocol Constants
// ============================================================================

// DNS header size in bytes
constexpr size_t DNS_HEADER_SIZE = 12;

// DNS port numbers
constexpr uint16_t DNS_DEFAULT_PORT = 53;
constexpr uint16_t DNS_PORT_MIN = 1;
constexpr uint16_t DNS_PORT_MAX = 65535;
constexpr uint16_t PRIVILEGED_PORT_MAX = 1023;

// DNS query types
constexpr uint16_t DNS_QTYPE_A = 1;      // IPv4 address
constexpr uint16_t DNS_QTYPE_AAAA = 28;  // IPv6 address
constexpr uint16_t DNS_QTYPE_MX = 15;    // Mail exchange
constexpr uint16_t DNS_QTYPE_NS = 2;     // Name server

// DNS query classes
constexpr uint16_t DNS_QCLASS_IN = 1;    // Internet

// DNS response codes (RCODE)
constexpr uint8_t DNS_RCODE_NOERROR = 0;   // No error
constexpr uint8_t DNS_RCODE_FORMERR = 1;   // Format error
constexpr uint8_t DNS_RCODE_SERVFAIL = 2;  // Server failure
constexpr uint8_t DNS_RCODE_NXDOMAIN = 3;  // Non-existent domain
constexpr uint8_t DNS_RCODE_NOTIMP = 4;    // Not implemented
constexpr uint8_t DNS_RCODE_REFUSED = 5;   // Query refused

// DNS flags
constexpr uint16_t DNS_FLAG_QR = 0x8000;   // Query/Response flag
constexpr uint16_t DNS_FLAG_AA = 0x0400;   // Authoritative answer
constexpr uint16_t DNS_FLAG_RD = 0x0100;   // Recursion desired
constexpr uint16_t DNS_FLAG_RA = 0x0080;   // Recursion available

// DNS record count limits (for response validation)
constexpr uint16_t DNS_MAX_ANCOUNT = 100;  // Max answer records in response
constexpr uint16_t DNS_MAX_NSCOUNT = 100;  // Max authority records in response
constexpr uint16_t DNS_MAX_ARCOUNT = 100;  // Max additional records in response

// ============================================================================
// Domain Validation Constants (RFC 3696, RFC 1123)
// ============================================================================

// Maximum lengths per RFC specifications
constexpr size_t DOMAIN_MAX_LENGTH = 253;   // Total domain length
constexpr size_t DOMAIN_LABEL_MAX = 63;     // Single label length
constexpr size_t DOMAIN_MIN_LENGTH = 1;     // Minimum valid domain

// ============================================================================
// Application Constants
// ============================================================================

// Buffer sizes
constexpr size_t LOG_BUFFER_SIZE = 1024;    // Logging format buffer
constexpr size_t DNS_BUFFER_SIZE = 512;     // DNS message buffer (RFC minimum)
constexpr size_t ADDR_STRING_MAX = 46;      // Max for IPv6 string (INET6_ADDRSTRLEN)

// Timeout and cleanup
constexpr int ID_MAPPING_TIMEOUT_SEC = 10;  // Cleanup stale mappings after 10s
constexpr int SOCKET_RECV_TIMEOUT_SEC = 1;  // Socket receive timeout

// URL scheme prefixes for optional stripping
const std::string URL_SCHEME_HTTP = "http://";
const std::string URL_SCHEME_HTTPS = "https://";
const std::string URL_SCHEME_FTP = "ftp://";

// ============================================================================
// Return Codes
// ============================================================================

constexpr int EXIT_SUCCESS_CODE = 0;
constexpr int EXIT_FAILURE_CODE = 1;

// ============================================================================
// Comment and Whitespace Characters
// ============================================================================

constexpr char COMMENT_CHAR = '#';
constexpr char DOMAIN_SEPARATOR = '.';
constexpr char LABEL_HYPHEN = '-';

// Domain validation patterns
const std::string CONSECUTIVE_HYPHENS = "--";
const std::string CONSECUTIVE_DOTS = "..";

// Whitespace characters for trimming
const std::string WHITESPACE_CHARS = " \t\r\n";

#endif // COMMON_H