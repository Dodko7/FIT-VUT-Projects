#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP

#include "client.hpp"
#include "../messages/udp_message.hpp"
#include <string>
#include <map>
#include <chrono>
#include <netinet/in.h>
#include <set>

/**
 * Represents a UDP client for the chat application.
 * Handles sending and receiving messages over UDP, including retries and confirmations.
 */
class UdpClient : public Client {
public:
    /**
     * Constructor for UdpClient.
     * @param server The server IP address.
     * @param port The server port.
     * @param timeout Timeout for message confirmations (in milliseconds).
     * @param retries Maximum number of retries for unconfirmed messages.
     */
    UdpClient(const std::string& server, uint16_t port, uint16_t timeout, uint8_t retries);

    /**
     * Sends a message to the server.
     * @param msg The message to send, wrapped in a shared pointer.
     */
    void send_message(const std::shared_ptr<MessageBase>& msg) override;

    /**
     * Receives messages from the server.
     * @return A vector of received messages wrapped in shared pointers.
     */
    std::vector<std::shared_ptr<MessageBase>> receive_message() override;

    /**
     * Terminates the UDP client, closing the socket.
     */
    void terminate() override;

    /**
     * Gets the file descriptor of the socket.
     * @return The socket file descriptor.
     */
    int get_socket_fd() const override;

private:
    std::string server_ip_; // Server IP address
    uint16_t server_port_; // Server port
    uint16_t timeout_; // Timeout for message confirmations
    uint8_t max_retries_; // Maximum number of retries for unconfirmed messages
    uint16_t next_message_id_; // ID for the next message to send
    int socket_fd_; // Socket file descriptor
    sockaddr_in server_addr_; // Server address structure
    std::set<uint16_t> received_message_ids_; // Set of received message IDs to avoid duplicates

    /**
     * Represents a pending message waiting for confirmation.
     */
    struct PendingMessage {
        std::shared_ptr<UdpMessage> message; // The message being sent
        uint8_t retries_left; // Number of retries left for this message
        std::chrono::steady_clock::time_point sent_time; // Time when the message was last sent
    };

    std::map<uint16_t, PendingMessage> pending_messages_; // Map of pending messages by message ID

    /**
     * Waits for a confirmation for a specific message ID.
     * @param message_id The ID of the message to confirm.
     * @param is_text_format Whether the message is in text format.
     * @return True if confirmation is received, false otherwise.
     */
    bool wait_for_confirm(uint16_t message_id, bool is_text_format);

    /**
     * Sends a confirmation for a received message.
     * @param message_id The ID of the message to confirm.
     * @param server_addr The address of the server to send the confirmation to.
     */
    void send_confirm(uint16_t message_id, const sockaddr_in& server_addr);
};

#endif // UDP_CLIENT_HPP