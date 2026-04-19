#include "tcp_message.hpp"
#include "../common/debug.hpp"
#include <sstream>
#include <stdexcept>

/**
 * @brief Constructs a TCP message from a content string.
 * @param content The message content.
 */
TcpMessage::TcpMessage(const std::string& content) 
    : type_(determine_type(content)), content_(content), is_success_(false) {
    printf_debug("Initialized TCP message: %s", content.c_str());
}

/**
 * @brief Parses a raw TCP message string into a TcpMessage object.
 * @param raw_message The raw message string.
 * @return Parsed TcpMessage object.
 * @throws std::runtime_error for invalid formats.
 */
TcpMessage TcpMessage::parse(const std::string& raw_message) {
    printf_debug("Parsing TCP message: %s", raw_message.c_str());
    // Check for proper \r\n termination
    if (raw_message.length() < 2 || raw_message.substr(raw_message.length() - 2) != "\r\n") {
        throw std::runtime_error("Invalid TCP message: missing \\r\\n");
    }

    std::string content = raw_message.substr(0, raw_message.length() - 2);
    TcpMessage msg(content);

    // Handle REPLY message parsing
    if (msg.type_ == Type::REPLY) {
        std::istringstream iss(content);
        std::string reply, status, is_keyword, message_content;
        iss >> reply >> status >> is_keyword;
        if (reply != "REPLY" || is_keyword != "IS" || (status != "OK" && status != "NOK")) {
            throw std::runtime_error("Invalid REPLY format");
        }
        std::getline(iss, message_content);
        if (!message_content.empty() && message_content[0] == ' ') {
            message_content = message_content.substr(1);
        }
        if (message_content.empty()) {
            throw std::runtime_error("Invalid REPLY format: missing content");
        }
        msg.is_success_ = (status == "OK");
        msg.content_ = message_content;
    } else if (msg.type_ == Type::MSG) {
        // Validate MSG format
        if (content.find("MSG FROM ") != 0 || content.find(" IS ") == std::string::npos) {
            throw std::runtime_error("Invalid MSG format");
        }
    } else if (msg.type_ == Type::ERR) {
        // Validate ERR format
        if (content.find("ERR FROM ") != 0 || content.find(" IS ") == std::string::npos) {
            throw std::runtime_error("Invalid ERR format");
        }
    } else if (msg.type_ == Type::AUTH) {
        // Validate AUTH format
        if (content.find("AUTH ") != 0 || content.find(" AS ") == std::string::npos || content.find(" USING ") == std::string::npos) {
            throw std::runtime_error("Invalid AUTH format");
        }
    } else if (msg.type_ == Type::BYE) {
        // Validate BYE format
        if (content != "BYE" && content.find("BYE FROM ") != 0) {
            throw std::runtime_error("Invalid BYE format");
        }
    } else if (msg.type_ == Type::JOIN) {
        // Validate JOIN format
        if (content.find("JOIN ") != 0 || content.find(" AS ") == std::string::npos) {
            throw std::runtime_error("Invalid JOIN format");
        }
    }

    return msg;
}

/**
 * @brief Formats the TCP message with \r\n termination.
 * @return Formatted message string.
 */
std::string TcpMessage::format() const {
    printf_debug("Formatting TCP message: %s", content_.c_str());
    return content_ + "\r\n";
}

/**
 * @brief Determines the message type based on its content.
 * @param content The message content.
 * @return Message type.
 * @throws std::runtime_error for unknown types.
 */
MessageBase::Type TcpMessage::determine_type(const std::string& content) {
    printf_debug("Determining TCP message type: %s", content.c_str());
    if (content.find("AUTH ") == 0) return Type::AUTH;
    if (content.find("MSG FROM ") == 0) return Type::MSG;
    if (content.find("REPLY ") == 0) return Type::REPLY;
    if (content.find("ERR FROM ") == 0) return Type::ERR;
    if (content == "BYE" || content.find("BYE FROM ") == 0) return Type::BYE;
    if (content.find("JOIN ") == 0) return Type::JOIN;
    throw std::runtime_error("Unknown TCP message type: " + content);
}