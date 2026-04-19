#ifndef MESSAGE_BASE_HPP
#define MESSAGE_BASE_HPP

#include <string>
#include <vector>
#include <cstdint>

/**
 * @class MessageBase
 * @brief Abstract base class for messages (TCP and UDP) in the IPK25-CHAT protocol.
 */
class MessageBase {
public:
    // Enum for message types as defined in the protocol.
    enum class Type : uint8_t {
        CONFIRM = 0x00,
        REPLY   = 0x01,
        AUTH    = 0x02,
        MSG     = 0x03,
        JOIN    = 0x04,
        PING    = 0x05,
        ERR     = 0xFE,
        BYE     = 0xFF
    };

    virtual ~MessageBase() = default;
    // Returns the message type.
    virtual Type get_type() const = 0;
    // Returns the message content.
    virtual std::string get_content() const = 0;
    // Indicates if the message (e.g., REPLY) was successful.
    virtual bool is_success() const { return false; }
};

#endif // MESSAGE_BASE_HPP