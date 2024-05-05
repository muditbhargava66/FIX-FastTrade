#include "model/Order.h"
#include <stdexcept>

namespace model {

Order::Order(const std::string& id, const std::string& symbol, double price, int quantity,
             Side side, OrderType type)
    : id_(id), symbol_(symbol), price_(price), quantity_(quantity),
      side_(side), type_(type), status_(OrderStatus::New) {}

const std::string& Order::getId() const {
    return id_;
}

const std::string& Order::getSymbol() const {
    return symbol_;
}

double Order::getPrice() const {
    return price_;
}

int Order::getQuantity() const {
    return quantity_;
}

Order::Side Order::getSide() const {
    return side_;
}

Order::OrderType Order::getType() const {
    return type_;
}

Order::OrderStatus Order::getStatus() const {
    return status_;
}

void Order::setQuantity(int quantity) {
    if (quantity < 0) {
        throw std::invalid_argument("Quantity cannot be negative");
    }
    quantity_ = quantity;
}

void Order::setStatus(OrderStatus status) {
    status_ = status;
}

bool Order::isFilled() const {
    return status_ == OrderStatus::Filled;
}

bool Order::isPartiallyFilled() const {
    return status_ == OrderStatus::PartiallyFilled;
}

bool Order::isCancelled() const {
    return status_ == OrderStatus::Cancelled;
}

}  // namespace model