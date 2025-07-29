#include "service/TradingService.h"

namespace service {

bool TradingService::processOrder(const model::Order& order) {
    if (!running_) {
        return false;
    }
    
    // Simple order processing - create a trade
    model::Trade::Side tradeSide = (order.getSide() == model::Order::Side::Buy) ? 
        model::Trade::Side::Buy : model::Trade::Side::Sell;
    
    model::Trade trade(
        "TRADE_" + order.getId(),
        order.getId(),
        order.getSymbol(),
        order.getPrice(),
        order.getQuantity(),
        tradeSide,
        std::chrono::system_clock::now()
    );
    
    trades_.push_back(trade);
    return true;
}

std::vector<model::Trade> TradingService::getTrades() const {
    return trades_;
}

void TradingService::start() {
    running_ = true;
}

void TradingService::stop() {
    running_ = false;
}

}  // namespace service