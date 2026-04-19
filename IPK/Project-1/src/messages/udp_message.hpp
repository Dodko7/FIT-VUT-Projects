#ifndef UDP_MESSAGE_HPP
#define UDP_MESSAGE_HPP

#include "message_base.hpp"
#include <vector>
#include <string>

/**
 * Represents a UDP message in the chat application.
 * Handles parsing and formatting of messages for UDP communication.
 */
class UdpMessage : public MessageBase {
public:
    /**
     * Constructor for UdpMessage.
     * @param type The type of the message (e.g., AUTH, MSG).
     * @param message_id The unique ID of the message.
     * @param content The content of the message.
     * @param use_text_format Whether the message uses text format.
     */
    UdpMessage(Type type, uint16_t message_id, const std::string& content, bool use_text_format = false);

    /**
     * Constructor for UdpMessage with success status.
     * @param type The type of the message (e.g., AUTH, MSG).
     * @param message_id The unique ID of the message.
     * @param content The content of the message.
     * @param is_success Whether the message indicates success.
     * @param use_text_format Whether the message uses text format.
     */
    UdpMessage(Type type, uint16_t message_id, const std::string& content, bool is_success, bool use_text_format);

    /**
     * Parses a raw UDP message into a UdpMessage object.
     * @param raw_message The raw message data as a vector of bytes.
     * @return A UdpMessage object.
     */
    static UdpMessage parse(const std::vector<uint8_t>& raw_message);

    /**
     * Formats the UdpMessage into a vector of bytes for transmission.
     * @return The formatted message as a vector of bytes.
     */
    std::vector<uint8_t> format() const;

    /**
     * Gets the type of the message.
     * @return The message type.
     */
    Type get_type() const override { return type_; }

    /**
     * Gets the content of the message.
     * @return The message content as a string.
     */
    std::string get_content() const override { return content_; }

    /**
     * Checks if the message indicates success.
     * @return True if the message indicates success, false otherwise.
     */
    bool is_success() const override { return is_success_; }

    /**
     * Gets the unique ID of the message.
     * @return The message ID.
     */
    uint16_t get_message_id() const { return message_id_; }

    /**
     * Checks if the message uses text format.
     * @return True if the message uses text format, false otherwise.
     */
    bool is_text_format() const { return use_text_format_; }

private:
    Type type_; // The type of the message
    uint16_t message_id_; // The unique ID of the message
    std::string content_; // The content of the message
    bool is_success_; // Indicates if the message is successful
    bool use_text_format_; // Indicates if the message uses text format

    /**
     * Parses a raw text message into a UdpMessage object.
     * @param raw_str The raw message as a string.
     * @param message_id The unique ID of the message.
     * @return A UdpMessage object.
     */
    static UdpMessage parse_text(const std::string& raw_str, uint16_t message_id);
};

#endif // UDP_MESSAGE_HPP