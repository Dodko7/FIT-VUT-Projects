/**
 * @file logger.cpp
 * @brief Implementation of centralized logging system
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 */

#include "../include/logger.h"
#include "../include/arg_parser.h"  // For Config struct
#include <cstdarg>
#include <cstdio>
#include <sstream>

// ============================================================================
// Static Member Initialization
// ============================================================================
// Initialize static verbose flag to false (conservative default)
bool Logger::verbose_enabled = false;

// ============================================================================
// Public Interface Implementation
// ============================================================================

void Logger::init(bool verbose) {
    verbose_enabled = verbose;
    
    // Log initialization status for debugging
    if (verbose) {
        std::cerr << "INFO: Verbose logging enabled" << std::endl;
    }
}

std::string Logger::format_message(LogLevel level, const std::string& message) {
    std::ostringstream oss;
    
    // Add severity prefix based on log level
    switch (level) {
        case LogLevel::ERROR:
            oss << "ERROR: ";
            break;
        case LogLevel::WARNING:
            oss << "WARNING: ";
            break;
        case LogLevel::INFO:
            oss << "INFO: ";
            break;
    }
    
    // Append the actual message content
    oss << message;
    return oss.str();
}

// ============================================================================
// String-based Logging Methods
// ============================================================================

void Logger::error(const std::string& message) {
    // Critical errors are always displayed regardless of verbose setting
    std::cerr << format_message(LogLevel::ERROR, message) << std::endl;
}

void Logger::warning(const std::string& message) {
    // Warnings are always displayed regardless of verbose setting
    std::cerr << format_message(LogLevel::WARNING, message) << std::endl;
}

void Logger::info(const std::string& message) {
    // Info messages only displayed when verbose mode is enabled
    if (verbose_enabled) {
        std::cerr << format_message(LogLevel::INFO, message) << std::endl;
    }
    // Silent when verbose disabled - no overhead
}

// ============================================================================
// Printf-style Formatted Logging Methods
// ============================================================================

void Logger::error_f(const char* format, ...) {
    // Use fixed-size buffer for formatting (defined in common.h)
    char buffer[LOG_BUFFER_SIZE];
    
    // Format the message using variable arguments
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Log the formatted message as error
    error(std::string(buffer));
}

void Logger::warning_f(const char* format, ...) {
    // Use fixed-size buffer for formatting
    char buffer[LOG_BUFFER_SIZE];
    
    // Format the message using variable arguments
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Log the formatted message as warning
    warning(std::string(buffer));
}

void Logger::info_f(const char* format, ...) {
    // Early return optimization - avoid formatting overhead if verbose disabled
    if (!verbose_enabled) {
        return;
    }
    
    // Use fixed-size buffer for formatting
    char buffer[LOG_BUFFER_SIZE];
    
    // Format the message using variable arguments
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Log the formatted message as info
    info(std::string(buffer));
}

// ============================================================================
// Specialized Logging Functions
// ============================================================================

void Logger::log_filter_stats(const LoadStats& stats, const std::string& filename) {
    // Only log statistics in verbose mode (INFO level handles this automatically)
    LOG_INFO_F("Filter file processed (%s): %zu total lines", filename.c_str(), stats.total_lines);
    LOG_INFO_F("  Loaded domains:  %zu", stats.loaded_domains);
    LOG_INFO_F("  Comment lines:   %zu", stats.comment_lines);
    LOG_INFO_F("  Empty lines:     %zu", stats.empty_lines);
    LOG_INFO_F("  Invalid/skipped: %zu", stats.invalid_lines);
    LOG_INFO_F("  Duplicates:      %zu", stats.duplicate_lines);
}

void Logger::log_config_summary(const Config& config) {
    // Early return if not in verbose mode (optimization)
    if (!verbose_enabled) {
        return;
    }
    
    // Display formatted configuration summary
    LOG_INFO("=== Configuration Summary ===");
    LOG_INFO_F("  Server:      %s", config.server.c_str());
    LOG_INFO_F("  Port:        %u", config.port);
    LOG_INFO_F("  Filter file: %s", config.filter_file.c_str());
    LOG_INFO_F("  Timeout:     %d seconds", config.timeout);
    LOG_INFO_F("  Verbose:     %s", config.verbose ? "enabled" : "disabled");
    LOG_INFO("=============================");;
}