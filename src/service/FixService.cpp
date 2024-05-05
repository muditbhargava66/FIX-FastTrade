#include "service/FixService.h"
#include "fix/Session.h"
#include "fix/MessageParser.h"
#include "model/Order.h"
#include <iostream>

namespace service {

FixService::FixService(const fix::Config& config, fix::Application& application)
    : session_(config, application) {}

void FixService::start() {
    session_.connect();
}

void FixService::stop() {
    session_.disconnect();
}

void FixService::sendOrder(const model::Order& order) {
    fix::Message message;
    populateNewOrderSingle(message, order);
    session_.send(message);
}

void FixService::cancelOrder(const std::string& orderId) {
    fix::Message message;
    populateOrderCancelRequest(message, orderId);
    session_.send(message);
}

void FixService::populateNewOrderSingle(fix::Message& message, const model::Order& order) {
    message.setField(fix::Tags::MsgType, "D");
    message.setField(fix::Tags::ClOrdID, order.getId());
    message.setField(fix::Tags::Symbol, order.getSymbol());
    message.setField(fix::Tags::Side, static_cast<char>(order.getSide()));
    message.setField(fix::Tags::OrderQty, std::to_string(order.getQuantity()));
    message.setField(fix::Tags::Price, std::to_string(order.getPrice()));
    message.setField(fix::Tags::OrdType, static_cast<char>(order.getType()));
    message.setField(fix::Tags::TimeInForce, "0");  // Day order
}

void FixService::populateOrderCancelRequest(fix::Message& message, const std::string& orderId) {
    message.setField(fix::Tags::MsgType, "F");
    message.setField(fix::Tags::OrigClOrdID, orderId);
    message.setField(fix::Tags::ClOrdID, generateCancelOrderId(orderId));
    message.setField(fix::Tags::Side, "0");  // Default to "Buy" side
}

std::string FixService::generateCancelOrderId(const std::string& orderId) {
    return "Cancel_" + orderId;
}

void FixService::onOrderAccepted(const fix::Message& message) {
    std::string orderId = message.getField(fix::Tags::OrderID);
    std::cout << "Order accepted. OrderID: " << orderId << std::endl;
}

void FixService::onOrderRejected(const fix::Message& message) {
    std::string reason = message.getField(fix::Tags::Text);
    std::cout << "Order rejected. Reason: " << reason << std::endl;
}

void FixService::onOrderExecuted(const fix::Message& message) {
    std::string orderId = message.getField(fix::Tags::OrderID);
    std::string execId = message.getField(fix::Tags::ExecID);
    double price = std::stod(message.getField(fix::Tags::LastPx));
    int quantity = std::stoi(message.getField(fix::Tags::LastQty));
    std::cout << "Order executed. OrderID: " << orderId << ", ExecID: " << execId
              << ", Price: " << price << ", Quantity: " << quantity << std::endl;
}

void FixService::onOrderCanceled(const fix::Message& message) {
    std::string orderId = message.getField(fix::Tags::OrderID);
    std::cout << "Order canceled. OrderID: " << orderId << std::endl;
}

}  // namespace service