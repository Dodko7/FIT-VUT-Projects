/**
 * @file logger.h
 * @brief Centralized logging system for DNS proxy/filter
 * @author Jozef Ondrejicka (xondre16)
 * @date 2025-15-10
 *
 * Responsibilities:
 * - Thread-safe centralized logging with severity levels (ERROR, WARNING, INFO)
 * - Respect verbose mode (-v flag) for INFO level messages
 * - Provide both string-based and printf-style formatted logging
 * - Always output ERROR and WARNING to stderr regardless of verbose mode
 * - Specialized logging for filter file loading statistics
 * - Configuration summary display in verbose mode
 * - Support for filter loading statistics tracking and reporting
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include <string>
#include <iostream>

// Forward declarations for specialized logging functions
struct Config;

/**
 * @brief Statistics for filter file loading operations
 */
struct LoadStats {
    size_t total_lines = 0;
    size_t loaded_domains = 0;
    size_t comment_lines = 0;
    size_t empty_lines = 0;
    size_t invalid_lines = 0;
    size_t duplicate_lines = 0;
};

/**
 * @brief Log severity levels
 */
enum class LogLevel {
    ERROR,    // Critical errors - always shown
    WARNING,  // Warnings - always shown
    INFO      // Informational - only shown with -v flag
};

/**
 * @brief Centralized logger with verbose mode support
 *
 * Thread-safe static logger that respects verbose flag.
 * All output goes to stderr for consistency.
 */
class Logger {
private:
    static bool verbose_enabled;  // Global verbose flag

    // Format message with appropriate prefix
    static std::string format_message(LogLevel level, const std::string& message);

public:
    // Initialize logger - must be called before any logging
    static void init(bool verbose);

    // Check if verbose mode is active
    static bool is_verbose() { return verbose_enabled; }

    // String-based logging
    static void error(const std::string& message);
    static void warning(const std::string& message);
    static void info(const std::string& message);

    // Printf-style formatted logging
    static void error_f(const char* format, ...) __attribute__((format(printf, 1, 2)));
    static void warning_f(const char* format, ...) __attribute__((format(printf, 1, 2)));
    static void info_f(const char* format, ...) __attribute__((format(printf, 1, 2)));

    // ========================================================================
    // Specialized Logging Functions
    // ========================================================================
    
    /**
     * @brief Log filter file loading statistics
     * 
     * Displays a formatted summary of filter file processing results
     * including total lines processed, domains loaded, and various
     * categories of skipped lines (comments, empty, invalid, duplicates).
     * 
     * @param stats Loading statistics structure
     * @param filename Filter filename for context
     * 
     * @note Output only shown in verbose mode (INFO level)
     */
    static void log_filter_stats(const LoadStats& stats, const std::string& filename);
    
    /**
     * @brief Log configuration summary in verbose mode
     * 
     * Displays a formatted table of current program configuration
     * including server address, port, filter file, and verbose status.
     * Used for debugging and user confirmation.
     * 
     * @param config Configuration structure to display
     * 
     * @note Only displays output when verbose mode is enabled
     * @note Uses INFO level logging (respects verbose flag)
     */
    static void log_config_summary(const Config& config);
};

// ============================================================================
// Convenience Macros
// ============================================================================
#define LOG_ERROR(msg)           Logger::error(msg)
#define LOG_WARNING(msg)         Logger::warning(msg)
#define LOG_INFO(msg)            Logger::info(msg)

#define LOG_ERROR_F(fmt, ...)    Logger::error_f(fmt, ##__VA_ARGS__)
#define LOG_WARNING_F(fmt, ...)  Logger::warning_f(fmt, ##__VA_ARGS__)
#define LOG_INFO_F(fmt, ...)     Logger::info_f(fmt, ##__VA_ARGS__)

#endif // LOGGER_H