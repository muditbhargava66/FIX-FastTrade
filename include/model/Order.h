#ifndef MODEL_ORDER_H
#define MODEL_ORDER_H

#include <string>

namespace model {

class Order {
public:
    enum class Side { Buy, Sell };
    enum class OrderType { Market, Limit, Stop, StopLimit };
    enum class OrderStatus { New, Accepted, Rejected, PartiallyFilled, Filled, Cancelled };

    Order(const std::string& id, const std::string& symbol, double price, int quantity,
          Side side, OrderType type);

    const std::string& getId() const;
    const std::string& getSymbol() const;
    double getPrice() const;
    int getQuantity() const;
    Side getSide() const;
    OrderType getType() const;
    OrderStatus getStatus() const;

    void setQuantity(int quantity);
    void setStatus(OrderStatus status);

    bool isFilled() const;
    bool isPartiallyFilled() const;
    bool isCancelled() const;

private:
    std::string id_;
    std::string symbol_;
    double price_;
    int quantity_;
    Side side_;
    OrderType type_;
    OrderStatus status_;
};

}  // namespace model

#endif  // MODEL_ORDER_H