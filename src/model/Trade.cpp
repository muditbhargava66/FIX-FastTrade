#include "model/Trade.h"

namespace model {

Trade::Trade(const std::string& id, const std::string& orderId, const std::string& symbol,
             double price, int quantity, Side side, std::chrono::system_clock::time_point timestamp)
    : id_(id), orderId_(orderId), symbol_(symbol), price_(price), quantity_(quantity),
      side_(side), timestamp_(timestamp) {}

const std::string& Trade::getId() const {
    return id_;
}

const std::string& Trade::getOrderId() const {
    return orderId_;
}

const std::string& Trade::getSymbol() const {
    return symbol_;
}

double Trade::getPrice() const {
    return price_;
}

int Trade::getQuantity() const {
    return quantity_;
}

Trade::Side Trade::getSide() const {
    return side_;
}

std::chrono::system_clock::time_point Trade::getTimestamp() const {
    return timestamp_;
}

}  // namespace model