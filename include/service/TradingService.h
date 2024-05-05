#ifndef SERVICE_TRADINGSERVICE_H
#define SERVICE_TRADINGSERVICE_H

#include "model/Order.h"
#include "model/Trade.h"
#include "service/FixService.h"
#include <string>
#include <unordered_map>

namespace service {

class TradingService {
public:
    TradingService(FixService& fixService);

    void placeOrder(const model::Order& order);
    void cancelOrder(const std::string& orderId);

    void onOrderAccepted(const std::string& orderId);
    void onOrderRejected(const std::string& orderId);
    void onOrderExecuted(const std::string& orderId, double price, int quantity);
    void onOrderCanceled(const std::string& orderId);

private:
    std::string generateTradeId();
    void processTrade(const model::Trade& trade);

    FixService& fixService_;
    std::unordered_map<std::string, model::Order> orders_;
};

}  // namespace service

#endif  // SERVICE_TRADINGSERVICE_H