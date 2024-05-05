#include "fix/Application.h"
#include "fix/Session.h"
#include "fix/MessageParser.h"
#include "service/FixService.h"

namespace fix {

Application::Application(const Config& config, service::FixService& fixService)
    : config_(config), fixService_(fixService) {}

void Application::onCreate(const SessionID& sessionID) {
    // Perform any necessary setup or initialization when a session is created
    // e.g., subscribe to market data, load order books, etc.
    // You can use the fixService_ to interact with other components
    fixService_.onSessionCreated(sessionID);
}

void Application::onLogon(const SessionID& sessionID) {
    // Handle the logon event for the specified session
    // e.g., authenticate the counterparty, sync order books, etc.
    fixService_.onLogon(sessionID);
}

void Application::onLogout(const SessionID& sessionID) {
    // Handle the logout event for the specified session
    // e.g., cleanup resources, notify other components, etc.
    fixService_.onLogout(sessionID);
}

void Application::toAdmin(Message& message, const SessionID& sessionID) {
    // Customize or modify outgoing administrative messages
    // e.g., add custom fields, perform message validation, etc.
    fixService_.toAdmin(message, sessionID);
}

void Application::fromAdmin(const Message& message, const SessionID& sessionID)
    throw(FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon) {
    // Process incoming administrative messages
    // e.g., handle session-level messages, perform authentication, etc.
    fixService_.fromAdmin(message, sessionID);
}

void Application::toApp(Message& message, const SessionID& sessionID)
    throw(DoNotSend) {
    // Customize or modify outgoing application messages
    // e.g., add custom fields, perform message validation, etc.
    fixService_.toApp(message, sessionID);
}

void Application::fromApp(const Message& message, const SessionID& sessionID)
    throw(FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType) {
    // Process incoming application messages
    // e.g., handle order-related messages, update order books, etc.
    fixService_.fromApp(message, sessionID);
}

}  // namespace fix