#ifndef SERVICE_FIXSERVICE_H
#define SERVICE_FIXSERVICE_H

#include "fix/Application.h"
#include "fix/Session.h"
#include "model/Order.h"
#include <memory>

namespace service {

/**
 * @brief FIX Service for handling FIX protocol operations
 */
class FixService {
public:
    FixService(std::shared_ptr<fix::Application> application);
    virtual ~FixService() = default;

    /**
     * @brief Send a new order
     */
    bool sendNewOrder(const model::Order& order);
    
    /**
     * @brief Cancel an order
     */
    bool cancelOrder(const std::string& orderId);
    
    /**
     * @brief Start the service
     */
    void start();
    
    /**
     * @brief Stop the service
     */
    void stop();

private:
    std::shared_ptr<fix::Application> application_;
    std::unique_ptr<fix::Session> session_;
    bool running_ = false;
};

}  // namespace service

#endif  // SERVICE_FIXSERVICE_H