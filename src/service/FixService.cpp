#include "service/FixService.h"
#include "fix/ZeroCopyMessage.h"

namespace service {

FixService::FixService(std::shared_ptr<fix::Application> application)
    : application_(application) {
}

bool FixService::sendNewOrder(const model::Order& order) {
    // Create a FIX message for new order
    fix::Message message;
    message.setField(fix::Tags::MsgType, "D");  // New Order Single
    message.setField(fix::Tags::Symbol, order.getSymbol());
    
    // Convert Side enum to FIX values: Buy=1, Sell=2
    std::string sideValue = (order.getSide() == model::Order::Side::Buy) ? "1" : "2";
    message.setField(fix::Tags::Side, sideValue);
    
    message.setField(fix::Tags::OrderQty, std::to_string(order.getQuantity()));
    message.setField(fix::Tags::Price, std::to_string(order.getPrice()));
    
    // Convert OrderType enum to FIX values: Market=1, Limit=2, Stop=3, StopLimit=4
    std::string ordTypeValue;
    switch (order.getType()) {
        case model::Order::OrderType::Market: ordTypeValue = "1"; break;
        case model::Order::OrderType::Limit: ordTypeValue = "2"; break;
        case model::Order::OrderType::Stop: ordTypeValue = "3"; break;
        case model::Order::OrderType::StopLimit: ordTypeValue = "4"; break;
    }
    message.setField(fix::Tags::OrdType, ordTypeValue);
    
    // If we have a session, use it; otherwise call application directly for testing
    if (session_ && running_) {
        return session_->send(message);
    } else if (application_) {
        // For testing: call toApp directly
        fix::SessionID sessionId("TEST_SESSION");
        application_->toApp(message, sessionId);
        return true;
    }
    
    return false;
}

bool FixService::cancelOrder(const std::string& orderId) {
    // Create a FIX message for order cancel
    fix::Message message;
    message.setField(fix::Tags::MsgType, "F");  // Order Cancel Request
    message.setField(fix::Tags::OrigClOrdID, orderId);
    message.setField(fix::Tags::ClOrdID, "Cancel_" + orderId);
    
    // If we have a session, use it; otherwise call application directly for testing
    if (session_ && running_) {
        return session_->send(message);
    } else if (application_) {
        // For testing: call toApp directly
        fix::SessionID sessionId("TEST_SESSION");
        application_->toApp(message, sessionId);
        return true;
    }
    
    return false;
}

void FixService::start() {
    running_ = true;
    // TODO: Initialize session and connect
}

void FixService::stop() {
    running_ = false;
    if (session_) {
        session_->disconnect();
    }
}

}  // namespace service