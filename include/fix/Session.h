#ifndef FIX_SESSION_H
#define FIX_SESSION_H

#include "fix/Config.h"
#include "fix/Application.h"
#include "fix/Message.h"
#include <string>
#include <boost/asio.hpp>

namespace fix {

class Session {
public:
    Session(const Config& config, Application& application);

    void connect();
    void disconnect();
    void send(const Message& message);
    bool receive(Message& message);

private:
    void logMessage(const Message& message, bool sent);

    const Config& config_;
    Application& application_;
    bool connected_;

    boost::asio::io_context ioContext_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    std::string sessionID_;
};

}  // namespace fix

#endif  // FIX_SESSION_H