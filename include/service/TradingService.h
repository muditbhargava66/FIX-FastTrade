#ifndef SERVICE_TRADINGSERVICE_H
#define SERVICE_TRADINGSERVICE_H

#include "model/Order.h"
#include "model/Trade.h"
#include <memory>
#include <vector>

namespace service {

/**
 * @brief Trading Service for handling trading operations
 */
class TradingService {
public:
    TradingService() = default;
    virtual ~TradingService() = default;

    /**
     * @brief Process an order
     */
    bool processOrder(const model::Order& order);
    
    /**
     * @brief Get all trades
     */
    std::vector<model::Trade> getTrades() const;
    
    /**
     * @brief Start the service
     */
    void start();
    
    /**
     * @brief Stop the service
     */
    void stop();

private:
    std::vector<model::Trade> trades_;
    bool running_ = false;
};

}  // namespace service

#endif  // SERVICE_TRADINGSERVICE_H