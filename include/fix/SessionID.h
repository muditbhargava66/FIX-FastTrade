#ifndef FIX_SESSIONID_H
#define FIX_SESSIONID_H

#include <string>
#include <ostream>

namespace fix {

/**
 * @brief FIX Session identifier
 */
class SessionID {
public:
    // Default constructor
    SessionID() = default;
    
    // Constructor from string (for backward compatibility)
    SessionID(const std::string& sessionId) : sessionId_(sessionId) {}
    
    // Constructor with FIX version, sender, and target
    SessionID(const std::string& beginString, const std::string& senderCompID, const std::string& targetCompID)
        : beginString_(beginString), senderCompID_(senderCompID), targetCompID_(targetCompID) {
        // Create a composite session ID
        sessionId_ = beginString + ":" + senderCompID + "->" + targetCompID;
    }
    
    // Copy constructor
    SessionID(const SessionID& other) = default;
    
    // Assignment operator
    SessionID& operator=(const SessionID& other) = default;
    
    // Conversion to string
    operator std::string() const { return sessionId_; }
    
    // String conversion
    std::string toString() const { return sessionId_; }
    
    // Getters
    const std::string& getBeginString() const { return beginString_; }
    const std::string& getSenderCompID() const { return senderCompID_; }
    const std::string& getTargetCompID() const { return targetCompID_; }
    
    // Comparison operators
    bool operator==(const SessionID& other) const {
        return sessionId_ == other.sessionId_;
    }
    
    bool operator!=(const SessionID& other) const {
        return !(*this == other);
    }
    
    bool operator<(const SessionID& other) const {
        return sessionId_ < other.sessionId_;
    }
    
    // Stream operator for logging
    friend std::ostream& operator<<(std::ostream& os, const SessionID& sessionId) {
        return os << sessionId.sessionId_;
    }

private:
    std::string sessionId_;
    std::string beginString_;
    std::string senderCompID_;
    std::string targetCompID_;
};

}  // namespace fix

#endif  // FIX_SESSIONID_H