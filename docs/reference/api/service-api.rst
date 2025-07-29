Service API Reference
=====================

This document provides an overview of the Service API provided by the FIX-FastTrade system. The Service API defines the high-level services and operations available for interacting with the trading system.

TradingService
--------------

The ``TradingService`` class provides functionality for handling trading operations and managing trades.

Constructor
~~~~~~~~~~~

.. code-block:: cpp

TradingService();


Core Methods
~~~~~~~~~~~~

.. code-block:: cpp

// Process an order
bool processOrder(const model::Order& order);

// Get all trades
std::vector<model::Trade> getTrades() const;

// Service lifecycle
void start();
void stop();


Usage Example
~~~~~~~~~~~~~

.. code-block:: cpp

#include "service/TradingService.h"
#include "model/Order.h"

// Create and start the trading service
service::TradingService tradingService;
tradingService.start();

// Create an order
model::Order order("ORD001", "AAPL", 150.00, 100, 
                   model::Order::Side::Buy, 
                   model::Order::OrderType::Limit);

// Process the order
if (tradingService.processOrder(order)) {
    std::cout << "Order processed successfully" << std::endl;
}

// Get all trades
auto trades = tradingService.getTrades();
for (const auto& trade : trades) {
    std::cout << "Trade: " << trade.getId() << std::endl;
}

// Stop the service
tradingService.stop();


FixService
----------

The ``FixService`` class provides functionality for handling FIX protocol operations and integrating with FIX sessions.

Constructor
~~~~~~~~~~~

.. code-block:: cpp

FixService(std::shared_ptr<fix::Application> application);


Core Methods
~~~~~~~~~~~~

.. code-block:: cpp

// Send a new order via FIX protocol
bool sendNewOrder(const model::Order& order);

// Cancel an order via FIX protocol
bool cancelOrder(const std::string& orderId);

// Service lifecycle
void start();
void stop();


Usage Example
~~~~~~~~~~~~~

.. code-block:: cpp

#include "service/FixService.h"
#include "fix/Application.h"
#include "model/Order.h"

// Create a FIX application implementation
class MyApplication : public fix::Application {
public:
    void onCreate(const fix::SessionID& sessionID) override {
        std::cout << "Session created: " << sessionID.toString() << std::endl;
    }
    
    void onLogon(const fix::SessionID& sessionID) override {
        std::cout << "Session logged on: " << sessionID.toString() << std::endl;
    }
    
    void onLogout(const fix::SessionID& sessionID) override {
        std::cout << "Session logged out: " << sessionID.toString() << std::endl;
    }
    
    void toAdmin(fix::Message& message, const fix::SessionID& sessionID) override {
        // Handle administrative messages
    }
    
    void fromAdmin(const fix::Message& message, const fix::SessionID& sessionID) override {
        // Handle administrative messages
    }
    
    void toApp(fix::Message& message, const fix::SessionID& sessionID) override {
        // Handle application messages before sending
    }
    
    void fromApp(const fix::Message& message, const fix::SessionID& sessionID) override {
        // Handle incoming application messages
    }
};

// Create and use the FIX service
auto application = std::make_shared<MyApplication>();
service::FixService fixService(application);

// Start the service
fixService.start();

// Create and send an order
model::Order order("ORD001", "AAPL", 150.00, 100, 
                   model::Order::Side::Buy, 
                   model::Order::OrderType::Limit);

if (fixService.sendNewOrder(order)) {
    std::cout << "Order sent successfully" << std::endl;
}

// Cancel an order
if (fixService.cancelOrder("ORD001")) {
    std::cout << "Order cancelled successfully" << std::endl;
}

// Stop the service
fixService.stop();


Service Integration
-------------------

Combining Services
~~~~~~~~~~~~~~~~~~

The services can be used together to create a complete trading system:

.. code-block:: cpp

// Create services
service::TradingService tradingService;
auto application = std::make_shared<MyApplication>();
service::FixService fixService(application);

// Start services
tradingService.start();
fixService.start();

// Process orders through both services
model::Order order("ORD001", "AAPL", 150.00, 100, 
                   model::Order::Side::Buy, 
                   model::Order::OrderType::Limit);

// Process locally
tradingService.processOrder(order);

// Send via FIX
fixService.sendNewOrder(order);

// Stop services
fixService.stop();
tradingService.stop();


Design Notes
------------

Thread Safety
~~~~~~~~~~~~~

Both services are designed to be thread-safe and can be used in multi-threaded environments.

Resource Management
~~~~~~~~~~~~~~~~~~~

Services use RAII principles and properly manage their resources. Always call ``stop()`` before destroying service instances.

Error Handling
~~~~~~~~~~~~~~

All service methods return boolean values to indicate success or failure. Check return values for proper error handling.

For more detailed information on the Service API and its usage, please refer to the FIX-FastTrade documentation and the source code documentation.

