#ifndef MESSAGE_VALIDATOR_HPP
#define MESSAGE_VALIDATOR_HPP

#include <string>

/**
 * @class MessageValidator
 * @brief Validates message parameters according to the IPK25-CHAT protocol.
 */
class MessageValidator {
public:
    /**
     * @brief Validates username or channel ID.
     * @param value The value to validate.
     * @param max_length Maximum allowed length (default: 20).
     * @return True if valid, false otherwise.
     */
    static bool validate_username_or_channel(const std::string& value, size_t max_length = 20);

    /**
     * @brief Validates display name.
     * @param value The display name to validate.
     * @return True if valid, false otherwise.
     */
    static bool validate_display_name(const std::string& value);

    /**
     * @brief Validates secret.
     * @param value The secret to validate.
     * @return True if valid, false otherwise.
     */
    static bool validate_secret(const std::string& value);

    /**
     * @brief Validates message content.
     * @param value The message content to validate.
     * @return True if valid, false otherwise.
     */
    static bool validate_message_content(const std::string& value);
};

#endif // MESSAGE_VALIDATOR_HPP