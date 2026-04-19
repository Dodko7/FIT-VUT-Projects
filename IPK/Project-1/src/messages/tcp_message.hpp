#ifndef TCP_MESSAGE_HPP
#define TCP_MESSAGE_HPP

#include "message_base.hpp"

/**
 * @class TcpMessage
 * @brief Handles TCP messages for the IPK25-CHAT protocol.
 */
class TcpMessage : public MessageBase {
public:
    /**
     * @brief Constructs a TCP message from a content string.
     * @param content The message content.
     */
    TcpMessage(const std::string& content);

    /**
     * @brief Parses a raw TCP message string into a TcpMessage object.
     * @param raw_message The raw message string.
     * @return Parsed TcpMessage object.
     * @throws std::runtime_error for invalid formats.
     */
    static TcpMessage parse(const std::string& raw_message);

    /**
     * @brief Formats the TCP message with \r\n termination.
     * @return Formatted message string.
     */
    std::string format() const;

    // Returns the message type.
    Type get_type() const override { return type_; }
    // Returns the message content.
    std::string get_content() const override { return content_; }
    // Indicates if the message (e.g., REPLY) was successful.
    bool is_success() const override { return is_success_; }

private:
    Type type_;           // Message type
    std::string content_; // Message content
    bool is_success_;     // Success status for REPLY messages

    /**
     * @brief Determines the message type from its content.
     * @param content The message content.
     * @return Message type.
     * @throws std::runtime_error for unknown types.
     */
    static Type determine_type(const std::string& content);
};

#endif // TCP_MESSAGE_HPP