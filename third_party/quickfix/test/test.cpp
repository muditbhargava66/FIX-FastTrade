#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Log.h>
#include <iostream>

class MyApplication : public FIX::Application
{
public:
    void onCreate(const FIX::SessionID&) {}
    void onLogon(const FIX::SessionID&) {}
    void onLogout(const FIX::SessionID&) {}
    void toAdmin(FIX::Message&, const FIX::SessionID&) throw(FIX::DoNotSend) {}
    void toApp(FIX::Message&, const FIX::SessionID&) throw(FIX::DoNotSend) {}
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {}
    void fromApp(const FIX::Message&, const FIX::SessionID&) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {}
};

int main(int argc, char** argv)
{
    try
    {
        MyApplication myApp;
        FIX::SessionSettings settings("config.cfg");
        FIX::FileStoreFactory storeFactory(settings);
        FIX::ScreenLogFactory logFactory(settings);
        FIX::SocketInitiator initiator(myApp, storeFactory, settings, logFactory);

        std::cout << "QuickFIX initialized successfully!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}