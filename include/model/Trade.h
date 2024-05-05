#ifndef MODEL_TRADE_H
#define MODEL_TRADE_H

#include <string>
#include <chrono>

namespace model {

class Trade {
public:
    enum class Side { Buy, Sell };

    Trade(const std::string& id, const std::string& orderId, const std::string& symbol,
          double price, int quantity, Side side, std::chrono::system_clock::time_point timestamp);

    const std::string& getId() const;
    const std::string& getOrderId() const;
    const std::string& getSymbol() const;
    double getPrice() const;
    int getQuantity() const;
    Side getSide() const;
    std::chrono::system_clock::time_point getTimestamp() const;

private:
    std::string id_;
    std::string orderId_;
    std::string symbol_;
    double price_;
    int quantity_;
    Side side_;
    std::chrono::system_clock::time_point timestamp_;
};

}  // namespace model

#endif  // MODEL_TRADE_H