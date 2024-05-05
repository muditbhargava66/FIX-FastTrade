#include "service/TradingService.h"
#include "model/Order.h"
#include "model/Trade.h"
#include <iostream>

namespace service {

void TradingService::placeOrder(const model::Order& order) {
    // Validate and process the order
    if (order.getQuantity() <= 0 || order.getPrice() <= 0.0) {
        std::cout << "Invalid order quantity or price." << std::endl;
        return;
    }

    // Add the order to the internal order book
    orders_[order.getId()] = order;

    // Send the order to the exchange using the FixService
    fixService_.sendOrder(order);
}

void TradingService::cancelOrder(const std::string& orderId) {
    // Check if the order exists in the order book
    auto it = orders_.find(orderId);
    if (it == orders_.end()) {
        std::cout << "Order not found. OrderID: " << orderId << std::endl;
        return;
    }

    // Send the order cancellation request to the exchange using the FixService
    fixService_.cancelOrder(orderId);
}

void TradingService::onOrderAccepted(const std::string& orderId) {
    // Update the order status in the order book
    auto it = orders_.find(orderId);
    if (it != orders_.end()) {
        it->second.setStatus(model::Order::OrderStatus::Accepted);
    }
}

void TradingService::onOrderRejected(const std::string& orderId) {
    // Update the order status in the order book
    auto it = orders_.find(orderId);
    if (it != orders_.end()) {
        it->second.setStatus(model::Order::OrderStatus::Rejected);
    }
}

void TradingService::onOrderExecuted(const std::string& orderId, double price, int quantity) {
    // Update the order status and quantity in the order book
    auto it = orders_.find(orderId);
    if (it != orders_.end()) {
        model::Order& order = it->second;
        order.setQuantity(order.getQuantity() - quantity);

        if (order.getQuantity() == 0) {
            order.setStatus(model::Order::OrderStatus::Filled);
        } else {
            order.setStatus(model::Order::OrderStatus::PartiallyFilled);
        }

        // Create a trade object and process the trade
        model::Trade trade(generateTradeId(), orderId, order.getSymbol(), price, quantity,
                           order.getSide(), std::chrono::system_clock::now());
        processTrade(trade);
    }
}

void TradingService::onOrderCanceled(const std::string& orderId) {
    // Update the order status in the order book
    auto it = orders_.find(orderId);
    if (it != orders_.end()) {
        it->second.setStatus(model::Order::OrderStatus::Cancelled);
    }
}

std::string TradingService::generateTradeId() {
    // Generate a unique trade ID
    // (Implement your own logic here, e.g., using a counter or UUID)
    static int tradeIdCounter = 0;
    return "Trade_" + std::to_string(++tradeIdCounter);
}

void TradingService::processTrade(const model::Trade& trade) {
    // Process the trade (e.g., update positions, calculate P&L, etc.)
    std::cout << "Processing trade. TradeID: " << trade.getId()
              << ", Symbol: " << trade.getSymbol()
              << ", Price: " << trade.getPrice()
              << ", Quantity: " << trade.getQuantity() << std::endl;
}

}  // namespace service