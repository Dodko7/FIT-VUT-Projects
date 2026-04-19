#include "tcp_client.hpp"
#include "../common/debug.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept>

/**
 * @brief Constructs a TCP client and connects to the specified server.
 * @param server Server IP address.
 * @param port Server port.
 * @throws std::runtime_error on connection failure.
 */
TcpClient::TcpClient(const std::string& server, uint16_t port)
    : server_ip_(server), server_port_(port), buffer_("") {
    printf_debug("Initializing TcpClient: server=%s, port=%d", server.c_str(), port);
    connect_to_server();
}

/**
 * @brief Connects to the server using a non-blocking socket.
 * @throws std::runtime_error on connection failure.
 */
void TcpClient::connect_to_server() {
    printf_debug("Creating socket");
    // Create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }
    socket_.set_fd(sockfd);

    // Set socket to non-blocking mode for connection
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    // Configure server address
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port_);
    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid server address: " + server_ip_);
    }

    printf_debug("Connecting to server");
    // Attempt to connect
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (errno != EINPROGRESS) {
            throw std::runtime_error("Failed to connect: " + std::string(strerror(errno)));
        }
        // Poll for connection completion
        struct pollfd pfd;
        pfd.fd = sockfd;
        pfd.events = POLLOUT;
        int poll_result = poll(&pfd, 1, 5000);
        if (poll_result <= 0) {
            throw std::runtime_error("Connection timeout: " + std::string(strerror(errno)));
        }
        // Check for connection errors
        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error != 0) {
            throw std::runtime_error("Connection failed: " + std::string(strerror(so_error)));
        }
    }

    // Set socket back to blocking mode
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
    printf_debug("Connected to server");
}

/**
 * @brief Sends a TCP message to the server.
 * @param msg The message to send.
 * @throws std::runtime_error on send failure.
 */
void TcpClient::send_message(const std::shared_ptr<MessageBase>& msg) {
    // Ensure message is a TCP message
    auto tcp_msg = std::dynamic_pointer_cast<TcpMessage>(msg);
    if (!tcp_msg) {
        throw std::runtime_error("Invalid message type for TCP");
    }
    std::string formatted = tcp_msg->format();
    printf_debug("Sending TCP message: %s", formatted.c_str());
    // Send the formatted message
    ssize_t sent = send(socket_.get_fd(), formatted.c_str(), formatted.length(), 0);
    if (sent < 0) {
        throw std::runtime_error("Failed to send: " + std::string(strerror(errno)));
    }
    if (sent != static_cast<ssize_t>(formatted.length())) {
        throw std::runtime_error("Incomplete message sent");
    }
}

/**
 * @brief Receives messages from the server.
 * @return Vector of received messages.
 * @throws std::runtime_error on receive failure.
 */
std::vector<std::shared_ptr<MessageBase>> TcpClient::receive_message() {
    printf_debug("Receiving TCP message");
    std::vector<char> buffer(4096);
    // Receive data into buffer
    ssize_t received = recv(socket_.get_fd(), buffer.data(), buffer.size() - 1, 0);
    if (received < 0) {
        throw std::runtime_error("Failed to receive: " + std::string(strerror(errno)));
    } else if (received == 0) {
        // Server closed connection
        printf_debug("Server closed connection");
        terminate();
        return {std::make_shared<TcpMessage>("BYE")};
    }

    buffer[received] = '\0';
    buffer_ += buffer.data();
    printf_debug("Appended to buffer: %s", buffer.data());

    // Parse complete messages from buffer
    std::vector<std::shared_ptr<MessageBase>> messages;
    size_t pos;
    while ((pos = buffer_.find("\r\n")) != std::string::npos) {
        std::string raw_message = buffer_.substr(0, pos + 2);
        buffer_ = buffer_.substr(pos + 2);
        printf_debug("Parsed TCP message: %s", raw_message.c_str());
        messages.push_back(std::make_shared<TcpMessage>(TcpMessage::parse(raw_message)));
    }

    return messages;
}

/**
 * @brief Performs a graceful shutdown of the TCP connection.
 */
void TcpClient::graceful_shutdown() {
    printf_debug("Performing graceful shutdown");
    if (socket_.get_fd() >= 0) {
        // Signal end of sending data
        if (shutdown(socket_.get_fd(), SHUT_WR) < 0) {
            printf_debug("Shutdown failed: %s", strerror(errno));
        }

        // Wait for remaining data or connection closure
        struct pollfd pfd;
        pfd.fd = socket_.get_fd();
        pfd.events = POLLIN;
        int poll_result = poll(&pfd, 1, 100); // Wait max 100 ms
        if (poll_result > 0 && pfd.revents & POLLIN) {
            char buffer[1024];
            ssize_t received = recv(socket_.get_fd(), buffer, sizeof(buffer), 0);
            if (received > 0) {
                printf_debug("Received %zd bytes during shutdown", received);
            } else if (received == 0) {
                printf_debug("Server closed connection during shutdown");
            }
        } else if (poll_result < 0) {
            printf_debug("Poll failed during shutdown: %s", strerror(errno));
        }
    }
}

/**
 * @brief Terminates the TCP client connection.
 */
void TcpClient::terminate() {
    printf_debug("Terminating TCP client");
    graceful_shutdown();
    socket_ = Socket(-1); // RAII handles socket closure
}