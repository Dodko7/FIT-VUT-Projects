#include "message_validator.hpp"
#include <regex>

// Validates username or channel ID according to protocol constraints.
bool MessageValidator::validate_username_or_channel(const std::string& value, size_t max_length) {
    std::regex valid_chars("[a-zA-Z0-9_-]+");
    // Check if non-empty, within length, and matches allowed characters.
    return !value.empty() && value.length() <= max_length && std::regex_match(value, valid_chars);
}

// Validates display name according to protocol constraints.
bool MessageValidator::validate_display_name(const std::string& value) {
    if (value.empty() || value.length() > 20) {
        return false;
    }
    // Check if all characters are printable ASCII (0x21-7E).
    for (char c : value) {
        if (c < 0x21 || c > 0x7E) {
            return false;
        }
    }
    return true;
}

// Validates secret according to protocol constraints.
bool MessageValidator::validate_secret(const std::string& value) {
    std::regex valid_chars("[a-zA-Z0-9_-]+");
    // Check if non-empty, within length, and matches allowed characters.
    return !value.empty() && value.length() <= 128 && std::regex_match(value, valid_chars);
}

// Validates message content according to protocol constraints.
bool MessageValidator::validate_message_content(const std::string& value) {
    if (value.length() > 60000) {
        return false;
    }
    // Check if all characters are printable ASCII or newline.
    for (char c : value) {
        if (c != 0x0A && (c < 0x20 || c > 0x7E)) {
            return false;
        }
    }
    return true;
}