#include "fix/Session.h"
#include "fix/Config.h"
#include "fix/MessageParser.h"
#include <chrono>
#include <iostream>
#include <boost/asio.hpp>

namespace fix {

Session::Session(const Config& config, Application& application)
    : config_(config), application_(application), connected_(false),
      socket_(ioContext_), resolver_(ioContext_) {}

void Session::connect() {
    boost::asio::ip::tcp::resolver::query query(
        config_.getString("SocketHost"),
        config_.getString("SocketPort")
    );

    boost::asio::connect(socket_, resolver_.resolve(query));

    connected_ = true;
    application_.onLogon(sessionID_);
}

void Session::disconnect() {
    if (connected_) {
        socket_.close();
        connected_ = false;
        application_.onLogout(sessionID_);
    }
}

void Session::send(const Message& message) {
    if (!connected_) {
        throw std::runtime_error("Session not connected");
    }

    std::string messageString = MessageParser::compose(message);

    boost::asio::write(socket_, boost::asio::buffer(messageString));

    logMessage(message, true);
}

bool Session::receive(Message& message) {
    if (!connected_) {
        return false;
    }

    boost::asio::streambuf buffer;
    boost::system::error_code error;
    size_t bytesRead = boost::asio::read_until(socket_, buffer, "\001", error);

    if (error) {
        if (error == boost::asio::error::eof) {
            disconnect();
        }
        return false;
    }

    std::istream is(&buffer);
    std::string messageString(std::istreambuf_iterator<char>(is), {});

    try {
        message = MessageParser::parse(messageString);
        logMessage(message, false);
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to parse received message: " << ex.what() << std::endl;
        return false;
    }
}

void Session::logMessage(const Message& message, bool sent) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);

    std::cout << (sent ? "Sent" : "Received") << " message at " << std::ctime(&timestamp);
    std::cout << MessageParser::compose(message) << std::endl;
}

}  // namespace fix