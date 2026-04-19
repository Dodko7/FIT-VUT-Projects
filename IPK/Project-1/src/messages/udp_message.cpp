#include "udp_message.hpp"
#include "../common/debug.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iomanip>

/**
 * @brief Constructs a UDP message with type, message ID, content, and format flag.
 * @param type Message type.
 * @param message_id Unique message identifier.
 * @param content Message content.
 * @param use_text_format Whether to use text format (true) or binary (false).
 */
UdpMessage::UdpMessage(Type type, uint16_t message_id, const std::string& content, bool use_text_format)
    : type_(type), message_id_(message_id), content_(content), is_success_(false), use_text_format_(use_text_format) {
    printf_debug("Initialized UDP message: type=%d, id=%d, content=%s, text_format=%d",
                 static_cast<int>(type), message_id, content.c_str(), use_text_format_);
}

/**
 * @brief Constructs a UDP message with type, message ID, content, success status, and format flag.
 * @param type Message type.
 * @param message_id Unique message identifier.
 * @param content Message content.
 * @param is_success Success status for REPLY messages.
 * @param use_text_format Whether to use text format (true) or binary (false).
 */
UdpMessage::UdpMessage(Type type, uint16_t message_id, const std::string& content, bool is_success, bool use_text_format)
    : type_(type), message_id_(message_id), content_(content), is_success_(is_success), use_text_format_(use_text_format) {
    printf_debug("Initialized UDP message: type=%d, id=%d, content=%s, is_success=%d, text_format=%d",
                 static_cast<int>(type), message_id, content.c_str(), is_success, use_text_format_);
}

/**
 * @brief Parses a raw UDP message into a UdpMessage object.
 * @param raw_message The raw message bytes.
 * @return Parsed UdpMessage object.
 * @throws std::runtime_error for invalid formats.
 */
UdpMessage UdpMessage::parse(const std::vector<uint8_t>& raw_message) {
    printf_debug("Parsing UDP message: size=%zu", raw_message.size());

    // Check if the message appears to be text-based
    std::string raw_str(raw_message.begin(), raw_message.end());
    bool is_text = raw_str.length() >= 2 && raw_str.substr(raw_str.length() - 2) == "\r\n";
    if (!is_text) {
        is_text = std::all_of(raw_message.begin(), raw_message.end(),
                              [](uint8_t c) { return c >= 32 && c <= 126; });
    }

    if (is_text && raw_str.find("AUTH IS ") == 0) {
        uint16_t message_id = 0;
        if (raw_message.size() >= 3) {
            message_id = (raw_message[1] << 8) | raw_message[2];
        }
        return parse_text(raw_str, message_id);
    }

    // Binary format parsing
    if (raw_message.size() < 3) {
        throw std::runtime_error("Invalid UDP message: too short");
    }

    Type type = static_cast<Type>(raw_message[0]);
    uint16_t message_id = (raw_message[1] << 8) | raw_message[2];
    printf_debug("Parsed message: type=%d, id=%d", static_cast<int>(type), message_id);

    // Log raw bytes for debugging
    std::string byte_str;
    for (uint8_t b : raw_message) {
        char hex[4];
        snprintf(hex, sizeof(hex), "%02X ", b);
        byte_str += hex;
    }
    printf_debug("Raw message bytes: %s", byte_str.c_str());

    // Handle CONFIRM and PING messages
    if (type == Type::CONFIRM || type == Type::PING) {
        return UdpMessage(type, message_id, "", false);
    }

    // Split content by null terminators
    std::vector<std::string> parts;
    std::string current;
    for (size_t i = 3; i < raw_message.size(); ++i) {
        if (raw_message[i] == 0) {
            parts.push_back(current);
            current.clear();
        } else {
            current += raw_message[i];
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }

    std::string content;
    if (type == Type::AUTH) {
        if (parts.size() != 3) throw std::runtime_error("Invalid AUTH format");
        content = parts[0] + " AS " + parts[1] + " USING " + parts[2];
    } else if (type == Type::JOIN) {
        if (parts.size() != 2) throw std::runtime_error("Invalid JOIN format");
        content = "JOIN " + parts[0] + " AS " + parts[1];
    } else if (type == Type::MSG) {
        if (parts.size() != 2) throw std::runtime_error("Invalid MSG format");
        content = "FROM " + parts[0] + " IS " + parts[1];
    } else if (type == Type::ERR) {
        if (parts.size() != 2) throw std::runtime_error("Invalid ERR format");
        content = "FROM " + parts[0] + " IS " + parts[1];
    } else if (type == Type::BYE) {
        content = parts.empty() ? "" : "FROM " + parts[0];
    } else if (type == Type::REPLY) {
        // Relaxed parsing for REPLY to handle extra byte
        if (raw_message.size() < 6) {
            throw std::runtime_error("Invalid REPLY format: too short");
        }
        bool is_success = (raw_message[3] == 0x01); // 01 for OK, 00 for NOK
        // Skip status (1 byte) and null terminators (2 bytes or more)
        size_t content_start = 6; // Default start after type, id, status, null, null
        if (raw_message.size() > 6 && raw_message[5] != 0) {
            printf_debug("Unexpected byte in REPLY at index 5: %02X", raw_message[5]);
            content_start = 6; // Still start after extra byte
        }
        content = std::string(raw_message.begin() + content_start, raw_message.end() - 1); // Skip final null
        UdpMessage msg(type, message_id, content, is_success, false);
        return msg;
    } else {
        throw std::runtime_error("Unknown UDP message type");
    }

    UdpMessage msg(type, message_id, content, false);
    return msg;
}

/**
 * @brief Parses a text-based UDP message into a UdpMessage object.
 * @param raw_str The raw message string.
 * @param message_id Unique message identifier.
 * @return Parsed UdpMessage object.
 * @throws std::runtime_error for invalid formats.
 */
UdpMessage UdpMessage::parse_text(const std::string& raw_str, uint16_t message_id) {
    printf_debug("Parsing text-based UDP message: %s", raw_str.c_str());

    // Check if the message starts with "AUTH IS "
    if (raw_str.find("AUTH IS ") == 0) {
        size_t as_pos = raw_str.find(" AS ");
        size_t using_pos = raw_str.find(" USING ");
        if (as_pos == std::string::npos || using_pos == std::string::npos) {
            throw std::runtime_error("Invalid AUTH text format");
        }
        std::string username = raw_str.substr(8, as_pos - 8); // Skip "AUTH IS "
        std::string display_name = raw_str.substr(as_pos + 4, using_pos - (as_pos + 4));
        std::string secret = raw_str.substr(using_pos + 7);
        std::string content = username + " AS " + display_name + " USING " + secret;
        return UdpMessage(Type::AUTH, message_id, content, true);
    }

    throw std::runtime_error("Unsupported text-based message format");
}

/**
 * @brief Formats the UDP message into a binary or text format.
 * @return Formatted message bytes.
 * @throws std::runtime_error for invalid content.
 */
std::vector<uint8_t> UdpMessage::format() const {
    printf_debug("Formatting UDP message: type=%d, id=%d, content=%s, text_format=%d",
                 static_cast<int>(type_), message_id_, content_.c_str(), use_text_format_);
    std::vector<uint8_t> buffer;

    // Use text format for MSG, ERR, JOIN, BYE if specified
    if (use_text_format_ && (type_ == Type::MSG || type_ == Type::ERR || type_ == Type::JOIN || type_ == Type::BYE)) {
        std::string text;
        if (type_ == Type::MSG || type_ == Type::ERR) {
            text = (type_ == Type::MSG ? "MSG " : "ERR ") + content_;
        } else if (type_ == Type::JOIN) {
            text = content_;
        } else if (type_ == Type::BYE) {
            text = content_.empty() ? "BYE" : content_;
        }
        text += "\r\n";
        buffer.insert(buffer.end(), text.begin(), text.end());
        return buffer;
    }

    // Special handling for AUTH to use binary format
    if (type_ == Type::AUTH) {
        // Parse content: "username AS display_name USING secret"
        size_t as_pos = content_.find(" AS ");
        size_t using_pos = content_.find(" USING ");
        if (as_pos == std::string::npos || using_pos == std::string::npos) {
            throw std::runtime_error("Invalid AUTH content format");
        }
        std::string username = content_.substr(0, as_pos);
        std::string display_name = content_.substr(as_pos + 4, using_pos - (as_pos + 4));
        std::string secret = content_.substr(using_pos + 7);

        // Binary format: type, message_id, username\0display_name\0secret\0
        buffer.push_back(static_cast<uint8_t>(type_));
        buffer.push_back((message_id_ >> 8) & 0xFF);
        buffer.push_back(message_id_ & 0xFF);
        buffer.insert(buffer.end(), username.begin(), username.end());
        buffer.push_back(0);
        buffer.insert(buffer.end(), display_name.begin(), display_name.end());
        buffer.push_back(0);
        buffer.insert(buffer.end(), secret.begin(), secret.end());
        buffer.push_back(0);
        return buffer;
    }

    // Binary format for other messages
    buffer.push_back(static_cast<uint8_t>(type_));
    buffer.push_back((message_id_ >> 8) & 0xFF);
    buffer.push_back(message_id_ & 0xFF);

    if (type_ == Type::JOIN) {
        size_t as_pos = content_.find(" AS ");
        if (as_pos == std::string::npos) {
            throw std::runtime_error("Invalid JOIN content for UDP");
        }
        std::string channel = content_.substr(5, as_pos - 5);
        std::string display_name = content_.substr(as_pos + 4);
        buffer.insert(buffer.end(), channel.begin(), channel.end());
        buffer.push_back(0);
        buffer.insert(buffer.end(), display_name.begin(), display_name.end());
        buffer.push_back(0);
    } else if (type_ == Type::MSG || type_ == Type::ERR) {
        size_t is_pos = content_.find(" IS ");
        if (is_pos == std::string::npos) {
            throw std::runtime_error("Invalid MSG/ERR content for UDP");
        }
        std::string from = content_.substr(5, is_pos - 5);
        std::string msg_content = content_.substr(is_pos + 4);
        buffer.insert(buffer.end(), from.begin(), from.end());
        buffer.push_back(0);
        buffer.insert(buffer.end(), msg_content.begin(), msg_content.end());
        buffer.push_back(0);
    } else if (type_ == Type::REPLY) {
        buffer.push_back(is_success_ ? 0x01 : 0x00); // 01 for OK, 00 for NOK
        buffer.push_back(0); // Null terminator for status
        buffer.push_back(0); // Extra byte to match received format
        buffer.insert(buffer.end(), content_.begin(), content_.end());
        buffer.push_back(0);
    } else if (type_ == Type::BYE) {
        if (!content_.empty()) {
            std::string from = content_.substr(5);
            buffer.insert(buffer.end(), from.begin(), from.end());
            buffer.push_back(0);
        }
    } else if (type_ == Type::PING || type_ == Type::CONFIRM) {
        // No additional content needed
    }

    return buffer;
}