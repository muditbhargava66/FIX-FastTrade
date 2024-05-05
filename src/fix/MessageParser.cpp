#include "fix/MessageParser.h"
#include <sstream>
#include <stdexcept>

namespace fix {

Message MessageParser::parse(const std::string& messageString) {
    Message message;
    std::istringstream iss(messageString);
    std::string field;

    while (std::getline(iss, field, '\001')) {
        size_t equalPos = field.find('=');
        if (equalPos == std::string::npos) {
            throw std::runtime_error("Invalid FIX field: " + field);
        }

        int tag = std::stoi(field.substr(0, equalPos));
        std::string value = field.substr(equalPos + 1);

        message.setField(tag, value);
    }

    return message;
}

std::string MessageParser::compose(const Message& message) {
    std::ostringstream oss;

    // Compose the message fields
    for (const auto& field : message.getFields()) {
        oss << field.first << '=' << field.second << '\001';
    }

    // Add the message trailer
    oss << "10=" << calculateChecksum(oss.str()) << '\001';

    return oss.str();
}

std::string MessageParser::calculateChecksum(const std::string& message) {
    int checksum = 0;
    for (char ch : message) {
        checksum += static_cast<int>(ch);
    }
    checksum %= 256;
    return std::to_string(checksum);
}

}  // namespace fix