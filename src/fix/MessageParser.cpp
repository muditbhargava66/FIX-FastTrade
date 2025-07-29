#include "fix/MessageParser.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace fix {

Message MessageParser::parse(const std::string& messageString) {
    Message message;
    
    // Determine field separator (SOH \001 or pipe | for testing)
    char separator = messageString.find('\001') != std::string::npos ? '\001' : '|';
    
    std::istringstream iss(messageString);
    std::string field;

    while (std::getline(iss, field, separator)) {
        if (field.empty()) continue;
        
        size_t equalPos = field.find('=');
        if (equalPos == std::string::npos) {
            continue; // Skip invalid fields instead of throwing
        }

        try {
            int tag = std::stoi(field.substr(0, equalPos));
            std::string value = field.substr(equalPos + 1);
            message.setField(tag, value);
        } catch (const std::exception&) {
            // Skip invalid tag numbers
            continue;
        }
    }

    return message;
}

std::string MessageParser::compose(const Message& message) {
    std::ostringstream oss;

    // Compose the message fields in proper FIX order
    const auto& fields = message.getFields();
    
    // Standard FIX field order (simplified)
    std::vector<int> fieldOrder = {8, 9, 35, 49, 56, 34, 52, 11, 21, 55, 54, 60, 38, 40, 44};
    
    for (int tag : fieldOrder) {
        auto it = fields.find(tag);
        if (it != fields.end()) {
            oss << it->first << '=' << it->second << '\001';
        }
    }
    
    // Add any remaining fields
    for (const auto& field : fields) {
        if (std::find(fieldOrder.begin(), fieldOrder.end(), field.first) == fieldOrder.end()) {
            oss << field.first << '=' << field.second << '\001';
        }
    }

    // Calculate and add checksum
    std::string messageWithoutChecksum = oss.str();
    oss << "10=" << calculateChecksum(messageWithoutChecksum) << '\001';

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