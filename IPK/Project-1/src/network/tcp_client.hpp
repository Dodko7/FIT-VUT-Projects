#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include "client.hpp"
#include "../messages/tcp_message.hpp"
#include <string>
#include <unistd.h>

/**
 * @class Socket
 * @brief RAII wrapper for socket file descriptors to ensure proper cleanup.
 */
class Socket {
public:
    // Constructor with optional file descriptor
    Socket(int fd = -1) : fd_(fd) {}
    // Destructor closes the socket if open
    ~Socket() { if (fd_ >= 0) close(fd_); }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    // Move constructor
    Socket(Socket&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }
    // Move assignment operator
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (fd_ >= 0) close(fd_);
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }
    // Get the socket file descriptor
    int get_fd() const { return fd_; }
    // Set the socket file descriptor
    void set_fd(int fd) { fd_ = fd; }

private:
    int fd_; // Socket file descriptor
};

/**
 * @class TcpClient
 * @brief Implements a TCP client for the IPK25-CHAT protocol.
 */
class TcpClient : public Client {
public:
    // Constructor initializes connection to server
    TcpClient(const std::string& server, uint16_t port);
    // Send a message to the server
    void send_message(const std::shared_ptr<MessageBase>& msg) override;
    // Receive messages from the server
    std::vector<std::shared_ptr<MessageBase>> receive_message() override;
    // Terminate the client connection
    void terminate() override;
    // Get the socket file descriptor
    int get_socket_fd() const override { return socket_.get_fd(); }

private:
    // Establish connection to the server
    void connect_to_server();
    // Perform graceful shutdown of the connection
    void graceful_shutdown();

    Socket socket_; // RAII-managed socket
    std::string server_ip_; // Server IP address
    uint16_t server_port_; // Server port
    std::string buffer_; // Buffer for received data
};

#endif // TCP_CLIENT_HPP