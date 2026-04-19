#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../messages/message_base.hpp"
#include <string>
#include <vector>
#include <memory>

/**
 * @class Client
 * @brief Abstract base class for network clients (TCP and UDP).
 */
class Client {
public:
    virtual ~Client() = default;
    // Sends a message to the server.
    virtual void send_message(const std::shared_ptr<MessageBase>& msg) = 0;
    // Receives messages from the server.
    virtual std::vector<std::shared_ptr<MessageBase>> receive_message() = 0;
    // Terminates the client connection.
    virtual void terminate() = 0;
    // Returns the socket file descriptor.
    virtual int get_socket_fd() const = 0;
};

#endif // CLIENT_HPP