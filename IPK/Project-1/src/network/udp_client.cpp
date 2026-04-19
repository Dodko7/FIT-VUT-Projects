#include "udp_client.hpp"
#include "../common/debug.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>

/**
 * Constructs a UDP client with specified parameters.
 */
UdpClient::UdpClient(const std::string& server, uint16_t port, uint16_t timeout, uint8_t retries)
    : server_ip_(server), server_port_(port), timeout_(timeout), max_retries_(retries), next_message_id_(0), socket_fd_(-1) {
    printf_debug("Initializing UdpClient: server=%s, port=%d, timeout=%d, retries=%d",
                 server.c_str(), port, timeout, retries);

    // Create a UDP socket
    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd_ < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    // Configure server address
    server_addr_ = {};
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(server_port_);
    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr_.sin_addr) <= 0) {
        close(socket_fd_);
        throw std::runtime_error("Invalid server address: " + server_ip_);
    }
}

/**
 * Sends a UDP message to the server with confirmation handling.
 */
void UdpClient::send_message(const std::shared_ptr<MessageBase>& msg) {
    auto udp_msg = std::dynamic_pointer_cast<UdpMessage>(msg);
    if (!udp_msg) {
        throw std::runtime_error("Invalid message type for UDP");
    }

    // Assign a new message ID, preserving the original text format
    udp_msg = std::make_shared<UdpMessage>(
        udp_msg->get_type(),
        next_message_id_++,
        udp_msg->get_content(),
        udp_msg->is_success(),
        udp_msg->is_text_format() // Preserve the original use_text_format_
    );
    std::vector<uint8_t> formatted = udp_msg->format();
    printf_debug("Sending UDP message: type=%d, id=%d, size=%zu",
                 static_cast<int>(udp_msg->get_type()), udp_msg->get_message_id(), formatted.size());
    printf_debug("Message content: %s", udp_msg->get_content().c_str());

    // Track pending message for confirmation
    PendingMessage pending = {udp_msg, max_retries_, std::chrono::steady_clock::now()};
    pending_messages_[udp_msg->get_message_id()] = pending;

    // Send the message
    if (sendto(socket_fd_, formatted.data(), formatted.size(), 0,
               (struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
        throw std::runtime_error("Failed to send: " + std::string(strerror(errno)));
    }

    // Wait for confirmation
    if (!wait_for_confirm(udp_msg->get_message_id(), udp_msg->is_text_format())) {
        throw std::runtime_error("Confirmation timeout after retries");
    }
}

/**
 * Waits for a confirmation for a specific message ID.
 */
bool UdpClient::wait_for_confirm(uint16_t message_id, bool is_text_format) {
    auto it = pending_messages_.find(message_id);
    if (it == pending_messages_.end()) {
        printf_debug("No pending message found for id=%d", message_id);
        return false;
    }

    // Retry sending until confirmation is received or retries are exhausted
    while (it->second.retries_left > 0) {
        struct pollfd pfd;
        pfd.fd = socket_fd_;
        pfd.events = POLLIN;
        int poll_result = poll(&pfd, 1, timeout_);
        if (poll_result > 0 && pfd.revents & POLLIN) {
            std::vector<uint8_t> buffer(4096);
            sockaddr_in sender_addr;
            socklen_t addr_len = sizeof(sender_addr);
            ssize_t received = recvfrom(socket_fd_, buffer.data(), buffer.size(), 0,
                                        (struct sockaddr*)&sender_addr, &addr_len);
            if (received < 0) {
                printf_debug("Receive error: %s", strerror(errno));
                continue;
            }
            buffer.resize(received);

            try {
                UdpMessage msg = UdpMessage::parse(buffer);
                printf_debug("Received message: type=%d, id=%d",
                             static_cast<int>(msg.get_type()), msg.get_message_id());

                // Verify sender address
                if (sender_addr.sin_addr.s_addr != server_addr_.sin_addr.s_addr ||
                    (server_addr_.sin_port != 0 && sender_addr.sin_port != server_addr_.sin_port)) {
                    printf_debug("Received message from unexpected sender: %s:%d",
                                 inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port));
                    continue;
                }

                // Check if confirmation matches the message ID or is acceptable for text messages
                if (msg.get_type() == MessageBase::Type::CONFIRM &&
                    (msg.get_message_id() == message_id || is_text_format)) {
                    pending_messages_.erase(message_id);
                    server_addr_ = sender_addr; // Update server address for dynamic port
                    printf_debug("Confirmed message id=%d (text_format=%d)", message_id, is_text_format);
                    return true;
                } else {
                    printf_debug("Received unexpected message: type=%d, id=%d",
                                 static_cast<int>(msg.get_type()), msg.get_message_id());
                }
            } catch (const std::exception& e) {
                printf_debug("Invalid confirm message: %s", e.what());
            }
        } else if (poll_result == 0) {
            printf_debug("Timeout waiting for confirm id=%d, retries left=%d",
                         message_id, it->second.retries_left);
        } else {
            printf_debug("Poll error: %s", strerror(errno));
        }

        if (it->second.retries_left > 0) {
            // Resend the message on timeout
            it->second.retries_left--;
            it->second.sent_time = std::chrono::steady_clock::now();
            std::vector<uint8_t> formatted = it->second.message->format();
            printf_debug("Resending message id=%d, retries left=%d", message_id, it->second.retries_left);
            if (sendto(socket_fd_, formatted.data(), formatted.size(), 0,
                       (struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
                printf_debug("Failed to resend: %s", strerror(errno));
            }
        }
    }

    printf_debug("Failed to receive confirm for id=%d after retries", message_id);
    pending_messages_.erase(message_id);
    return false;
}

/**
 * Sends a confirmation for a received message.
 */
void UdpClient::send_confirm(uint16_t message_id, const sockaddr_in& server_addr) {
    UdpMessage confirm(MessageBase::Type::CONFIRM, message_id, "", false);
    std::vector<uint8_t> formatted = confirm.format();
    printf_debug("Sending CONFIRM for message ID=%d", message_id);
    if (sendto(socket_fd_, formatted.data(), formatted.size(), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf_debug("Failed to send CONFIRM: %s", strerror(errno));
    }
}

/**
 * Receives UDP messages from the server.
 */
std::vector<std::shared_ptr<MessageBase>> UdpClient::receive_message() {
    printf_debug("Receiving UDP message");
    std::vector<uint8_t> buffer(4096);
    sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    // Receive a datagram
    ssize_t received = recvfrom(socket_fd_, buffer.data(), buffer.size(), 0,
                                (struct sockaddr*)&sender_addr, &addr_len);
    if (received < 0) {
        throw std::runtime_error("Failed to receive: " + std::string(strerror(errno)));
    }
    buffer.resize(received);
    printf_debug("Received raw message: size=%zu, content=%s",
                 buffer.size(), std::string(buffer.begin(), buffer.end()).c_str());
    // Log raw bytes in hex
    std::stringstream hex_ss;
    for (const auto& byte : buffer) {
        hex_ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    }
    printf_debug("Received raw message bytes: %s", hex_ss.str().c_str());

    try {
        UdpMessage msg = UdpMessage::parse(buffer);
        printf_debug("Received message: type=%d, id=%d, content=%s",
                     static_cast<int>(msg.get_type()), msg.get_message_id(), msg.get_content().c_str());

        // Check for duplicate messages
        if (received_message_ids_.find(msg.get_message_id()) != received_message_ids_.end()) {
            printf_debug("Duplicate message id=%d, sending confirm", msg.get_message_id());
            send_confirm(msg.get_message_id(), sender_addr);
            return {};
        }

        // Mark message as received and send confirmation
        received_message_ids_.insert(msg.get_message_id());
        send_confirm(msg.get_message_id(), sender_addr);
        server_addr_ = sender_addr;

        // Handle BYE message termination
        if (msg.get_type() == MessageBase::Type::BYE) {
            terminate();
        }

        return {std::make_shared<UdpMessage>(msg)};
    } catch (const std::exception& e) {
        printf_debug("Failed to parse received message: %s", e.what());
        return {};
    }
}

/**
 * Terminates the UDP client connection.
 */
void UdpClient::terminate() {
    printf_debug("Terminating UDP client");
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

/**
 * Returns the socket file descriptor.
 */
int UdpClient::get_socket_fd() const {
    return socket_fd_;
}