#ifndef FIX_MESSAGEPARSER_H
#define FIX_MESSAGEPARSER_H

#include "fix/Message.h"
#include <string>

namespace fix {

class MessageParser {
public:
    static Message parse(const std::string& messageString);
    static std::string compose(const Message& message);

private:
    static std::string calculateChecksum(const std::string& message);
};

}  // namespace fix

#endif  // FIX_MESSAGEPARSER_H