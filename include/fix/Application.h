#ifndef FIX_APPLICATION_H
#define FIX_APPLICATION_H

#include "fix/Message.h"
#include "fix/Session.h"

namespace fix {

class Application {
public:
    virtual ~Application() = default;

    virtual void onCreate(const SessionID& sessionID) = 0;
    virtual void onLogon(const SessionID& sessionID) = 0;
    virtual void onLogout(const SessionID& sessionID) = 0;

    virtual void toAdmin(Message& message, const SessionID& sessionID) = 0;
    virtual void fromAdmin(const Message& message, const SessionID& sessionID) = 0;

    virtual void toApp(Message& message, const SessionID& sessionID) = 0;
    virtual void fromApp(const Message& message, const SessionID& sessionID) = 0;
};

}  // namespace fix

#endif  // FIX_APPLICATION_H