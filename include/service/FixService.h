#ifndef SERVICE_FIXSERVICE_H
#define SERVICE_FIXSERVICE_H

#include "fix/Config.h"
#include "fix/Application.h"
#include "model/Order.h"
#include <string>

namespace fix {
class Session;
}

namespace service {

class FixService {
public:
    FixService(const fix::Config& config, fix::Application& application);

    void start();
    void stop();

    void sendOrder(const model::Order& order);
    void cancelOrder(const std::string& orderId);

    void onOrderAccepted(const fix::Message& message);
    void onOrderRejected(const fix::Message& message);
    void onOrderExecuted(const fix::Message& message);
    void onOrderCanceled(const fix::Message& message);

private:
    void populateNewOrderSingle(fix::Message& message, const model::Order& order);
    void populateOrderCancelRequest(fix::Message& message, const std::string& orderId);
    std::string generateCancelOrderId(const std::string& orderId);

    fix::Session& session_;
};

}  // namespace service

#endif  // SERVICE_FIXSERVICE_H