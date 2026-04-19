/**
 * @file filter.cpp
 * @brief Implementation of domain filtering logic
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 */

#include "../include/filter.h"
#include "../include/domain_utils.h"
#include "../include/logger.h"
#include <fstream>
#include <sstream>

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Process a single line from filter file
 * @param line Line to process
 * @param line_number Line number for error reporting
 * @param filename Filename for error reporting
 * @param blocked_domains Set to add valid domain to
 * @param stats Statistics to update
 */
static void process_filter_line(const std::string& line,
                                size_t line_number,
                                const std::string& filename,
                                std::unordered_set<std::string>& blocked_domains,
                                LoadStats& stats) {
    // Skip empty lines
    if (line.empty()) {
        stats.empty_lines++;
        return;
    }
    
    // Skip comment lines
    if (line[0] == COMMENT_CHAR) {
        stats.comment_lines++;
        return;
    }
    
    // Strip URL schemes for robustness
    std::string processed_line = DomainUtils::strip_url_scheme(line);
    
    // Normalize domain
    std::string domain = DomainUtils::normalize_domain(processed_line);
    
    // Check if normalization resulted in empty string
    if (domain.empty()) {
        LOG_WARNING_F("%s: line %zu: empty after normalization -- skipping", 
                        filename.c_str(), line_number);
        stats.invalid_lines++;
        return;
    }
    
    // Validate domain format
    if (!DomainUtils::validate_domain(domain)) {
        // Use exact format from copilot instructions
        LOG_WARNING_F("Warning: %s: line %zu: invalid domain '%s' -- skipping", 
                        filename.c_str(), line_number, line.c_str());
        stats.invalid_lines++;
        return;
    }
    
    // Check for duplicates
    if (blocked_domains.count(domain)) {
        LOG_WARNING_F("%s: line %zu: duplicate domain '%s' -- skipping", 
                        filename.c_str(), line_number, domain.c_str());
        stats.duplicate_lines++;
        return;
    }

    // Add valid domain to blocked set
    blocked_domains.insert(domain);

    // Update counter of successfully loaded domains
    stats.loaded_domains++;
}

// ============================================================================
// Main Filter Functions
// ============================================================================

bool DomainFilter::load_filter_file(const std::string& filename) {
    LOG_INFO_F("Loading filter file: %s", filename.c_str());
    
    // Clear any existing domains
    blocked_domains.clear();
    
    // Open filter file in binary mode to handle all line endings
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR_F("Failed to open filter file: %s", filename.c_str());
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Normalize line endings: Convert all \r\n and \r to \n
    // First replace \r\n with \n (Windows)
    size_t pos = 0;
    while ((pos = content.find("\r\n", pos)) != std::string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1;
    }
    // Then replace remaining \r with \n (old Mac)
    pos = 0;
    while ((pos = content.find('\r', pos)) != std::string::npos) {
        content[pos] = '\n';
        pos += 1;
    }
    
    // Initialize statistics
    LoadStats stats;
    
    // Process file line by line using normalized content
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        stats.total_lines++;
        
        // Process the line (no need to remove \r anymore)
        process_filter_line(line, stats.total_lines, filename, 
                            blocked_domains, stats);
    }
    
    // Log statistics using centralized logger
    Logger::log_filter_stats(stats, filename);
    
    // Check if we loaded any domains
    if (blocked_domains.empty()) {
        LOG_WARNING("Filter file contains no valid domains");
        return false;
    }
    
    LOG_INFO_F("Filter file loaded successfully: %zu domains ready", 
                stats.loaded_domains);

    return true;
}

bool DomainFilter::is_blocked(const std::string& domain) const {
    // Normalize for consistent comparison
    std::string normalized = DomainUtils::normalize_domain(domain);
    
    // Check if normalization resulted in empty string
    if (normalized.empty()) {
        LOG_WARNING_F("Empty domain after normalization: '%s'", domain.c_str());
        return false;
    }
    
    LOG_INFO_F("Checking domain: %s", normalized.c_str());
    
    // Fast lookup: O(1) exact match check
    if (blocked_domains.count(normalized)) {
        LOG_INFO_F("Domain blocked (exact match): %s", normalized.c_str());
        return true;
    }
    
    // Slow lookup: O(n) subdomain check
    for (const std::string& blocked : blocked_domains) {
        // Check if normalized domain is a subdomain of blocked domain
        if (DomainUtils::is_subdomain(normalized, blocked)) {
            LOG_INFO_F("Domain blocked (subdomain of %s): %s", 
                        blocked.c_str(), normalized.c_str());
            return true;
        }
    }
    
    // Domain is allowed
    LOG_INFO_F("Domain allowed: %s", normalized.c_str());

    return false;
}