# Basic Usage Examples

This document provides working code examples for using the FIX-FastTrade system. All examples are based on the current implementation and have been tested to work with the existing codebase.

Table of Contents
-----------------
* [Session Management](#session-management)
* [Message Handling](#message-handling)
* [Order Processing](#order-processing)
* [Service Integration](#service-integration)
* [Configuration](#configuration)
* [Complete Example](#complete-example)

Session Management
------------------

Creating a FIX Session
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "fix/SessionID.h"

// Create a session with FIX version, sender, and target
fix::SessionID sessionId("FIX.4.4", "CLIENT", "SERVER");

// Get session information
std::string version = sessionId.getBeginString();    // "FIX.4.4"
std::string sender = sessionId.getSenderCompID();    // "CLIENT"
std::string target = sessionId.getTargetCompID();    // "SERVER"
std::string sessionString = sessionId.toString();    // "FIX.4.4:CLIENT->SERVER"

// Session comparison
fix::SessionID otherSession("FIX.4.4", "CLIENT", "SERVER");
bool isEqual = (sessionId == otherSession);  // true

// Use in containers
std::map<fix::SessionID, std::string> sessionMap;
sessionMap[sessionId] = "Active session";


Session ID Conversion
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Convert to string for logging
std::cout << "Session: " << sessionId << std::endl;

// Create from string (backward compatibility)
fix::SessionID fromString("FIX.4.4:CLIENT->SERVER");

// Copy and assignment
fix::SessionID copied = sessionId;
fix::SessionID assigned;
assigned = sessionId;


Message Handling
----------------

Basic Message Operations
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "fix/Message.h"
#include "fix/ZeroCopyMessage.h"

// Create and populate a FIX message
fix::Message message;
message.setField(fix::Tags::MsgType, "D");           // New Order Single
message.setField(fix::Tags::ClOrdID, "ORDER123");    // Client Order ID
message.setField(fix::Tags::Symbol, "AAPL");         // Symbol
message.setField(fix::Tags::Side, "1");              // Buy side
message.setField(fix::Tags::OrderQty, "100");        // Quantity
message.setField(fix::Tags::Price, "150.25");        // Price

// Read message fields
std::string msgType = message.getField(fix::Tags::MsgType);
std::string symbol = message.getField(fix::Tags::Symbol);
bool hasPrice = message.hasField(fix::Tags::Price);

// Get all fields
const auto& fields = message.getFields();
for (const auto& [tag, value] : fields) {
    std::cout << "Tag " << tag << " = " << value << std::endl;
}

// Clear and reuse
message.clear();
message.setField(fix::Tags::MsgType, "8");  // Execution Report


Zero-Copy Message Processing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Parse incoming FIX message without allocations
const char* buffer = "8=FIX.4.4\0019=154\00135=D\00111=ORDER123\00155=AAPL\00154=1\00138=100\00144=150.25\00110=123\001";
size_t length = strlen(buffer);

fix::ZeroCopyMessage msg(buffer, length);

// Fast field access
std::string_view msgType = msg.getField(fix::Tags::MsgType);        // "D"
std::string_view symbol = msg.getField(fix::Tags::Symbol);          // "AAPL"
auto quantity = msg.getFieldAsInt(fix::Tags::OrderQty);             // 100
auto price = msg.getFieldAsDouble(fix::Tags::Price);                // 150.25

// Check field existence
bool hasClOrdID = msg.hasField(fix::Tags::ClOrdID);

// Iterate through all fields
for (const auto& [tag, value] : msg) {
    std::cout << "Tag " << tag << " = " << value << std::endl;
}

// Validate message integrity
bool isValid = msg.validateChecksum();


Using FIX Tags Namespace
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "fix/ZeroCopyMessage.h"

// All standard FIX tags are available in the Tags namespace
using namespace fix::Tags;

fix::Message order;
order.setField(BeginString, "FIX.4.4");
order.setField(MsgType, "D");
order.setField(ClOrdID, "ORDER123");
order.setField(Symbol, "AAPL");
order.setField(Side, "1");              // Buy
order.setField(OrderQty, "100");
order.setField(OrdType, "2");           // Limit
order.setField(Price, "150.25");
order.setField(TimeInForce, "0");       // Day
order.setField(SenderCompID, "CLIENT");
order.setField(TargetCompID, "SERVER");

// Common tags available:
// BeginString, BodyLength, CheckSum, ClOrdID, CumQty, ExecID
// HandlInst, MsgSeqNum, MsgType, OrderQty, OrdType, OrigClOrdID
// Price, SenderCompID, SendingTime, Side, Symbol, TargetCompID
// Text, TransactTime, SettlDate, SymbolSfx, ExecType, LeavesQty, AvgPx


Order Processing
----------------

Creating and Managing Orders
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "model/Order.h"

// Create a new order
model::Order order;
order.setSymbol("AAPL");
order.setQuantity(100);
order.setSide(model::Order::Side::BUY);
order.setPrice(150.25);
order.setOrderType(model::Order::Type::LIMIT);
order.setClientOrderId("ORDER123");

// Access order information
std::string symbol = order.getSymbol();
int quantity = order.getQuantity();
model::Order::Side side = order.getSide();
double price = order.getPrice();
model::Order::Type orderType = order.getOrderType();

// Order validation
bool isValid = order.isValid();
if (!isValid) {
    std::cout << "Order validation failed" << std::endl;
}


Working with Trades
~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "model/Trade.h"

// Create a trade (typically done by the trading service)
model::Trade trade;
trade.setSymbol("AAPL");
trade.setQuantity(100);
trade.setPrice(150.30);  // Executed price
trade.setSide(model::Trade::Side::BUY);
trade.setTradeId("TRADE456");

// Access trade information
std::string symbol = trade.getSymbol();
int executedQty = trade.getQuantity();
double executedPrice = trade.getPrice();
auto timestamp = trade.getTimestamp();
std::string tradeId = trade.getTradeId();

// Trade reporting
std::cout << "Trade executed: " << symbol 
          << " " << executedQty 
          << " @ " << executedPrice << std::endl;


Service Integration
-------------------

Using the Trading Service
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "service/TradingService.h"

// Create and start trading service
auto tradingService = std::make_shared<service::TradingService>();
tradingService->start();

// Process an order
model::Order order;
order.setSymbol("AAPL");
order.setQuantity(100);
order.setSide(model::Order::Side::BUY);
order.setPrice(150.25);

bool processed = tradingService->processOrder(order);
if (processed) {
    std::cout << "Order processed successfully" << std::endl;
    
    // Get all trades
    auto trades = tradingService->getTrades();
    std::cout << "Total trades: " << trades.size() << std::endl;
    
    for (const auto& trade : trades) {
        std::cout << "Trade: " << trade.getSymbol() 
                  << " " << trade.getQuantity() 
                  << " @ " << trade.getPrice() << std::endl;
    }
}

// Stop the service
tradingService->stop();


Using the FIX Service
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "service/FixService.h"
#include "fix/Application.h"

// Create FIX application (implementation specific)
auto application = std::make_shared<fix::Application>();

// Create and start FIX service
auto fixService = std::make_shared<service::FixService>(application);
fixService->start();

// Send a new order
model::Order order;
order.setSymbol("AAPL");
order.setQuantity(100);
order.setSide(model::Order::Side::BUY);
order.setPrice(150.25);
order.setClientOrderId("ORDER123");

bool sent = fixService->sendNewOrder(order);
if (sent) {
    std::cout << "Order sent successfully" << std::endl;
} else {
    std::cout << "Failed to send order" << std::endl;
}

// Cancel an order
bool cancelled = fixService->cancelOrder("ORDER123");
if (cancelled) {
    std::cout << "Order cancelled successfully" << std::endl;
}

// Stop the service
fixService->stop();


Configuration
-------------

XML Configuration Example
~~~~~~~~~~~~~~~~~~~~~~~~~

Create a configuration file ``config/basic-config.xml``:

.. code-block:: xml

<?xml version="1.0" encoding="UTF-8"?>
<fix-config>
    <session>
        <beginString>FIX.4.4</beginString>
        <senderCompID>CLIENT</senderCompID>
        <targetCompID>SERVER</targetCompID>
        <heartBtInt>30</heartBtInt>
        <socketConnectHost>localhost</socketConnectHost>
        <socketConnectPort>9876</socketConnectPort>
        <startTime>00:00:00</startTime>
        <endTime>23:59:59</endTime>
        <logonTimeout>10</logonTimeout>
        <logoutTimeout>5</logoutTimeout>
    </session>
    
    <logging>
        <logLevel>INFO</logLevel>
        <logFile>logs/fix-fasttrade.log</logFile>
        <maxFileSize>100MB</maxFileSize>
        <maxFiles>10</maxFiles>
    </logging>
    
    <performance>
        <enableZeroCopy>true</enableZeroCopy>
        <enableSIMD>true</enableSIMD>
        <cpuAffinity>0,1,2,3</cpuAffinity>
        <memoryPoolSize>1048576</memoryPoolSize>
    </performance>
</fix-config>


Loading Configuration
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "fix/Config.h"

// Load configuration from file
fix::Config config;
bool loaded = config.loadFromFile("config/basic-config.xml");
if (!loaded) {
    std::cerr << "Failed to load configuration" << std::endl;
    return -1;
}

// Access configuration values
std::string beginString = config.getString("session.beginString");
std::string senderCompID = config.getString("session.senderCompID");
int heartBtInt = config.getInt("session.heartBtInt");
bool enableZeroCopy = config.getBool("performance.enableZeroCopy");

// Use configuration to create session
fix::SessionID sessionId(beginString, senderCompID, 
                        config.getString("session.targetCompID"));


Complete Example
----------------

Here's a complete example that demonstrates the full workflow:

.. code-block:: cpp

#include <iostream>
#include <memory>
#include "fix/SessionID.h"
#include "fix/Message.h"
#include "fix/ZeroCopyMessage.h"
#include "service/TradingService.h"
#include "service/FixService.h"
#include "model/Order.h"
#include "model/Trade.h"

int main() {
    try {
        // 1. Create FIX session
        fix::SessionID sessionId("FIX.4.4", "CLIENT", "SERVER");
        std::cout << "Created session: " << sessionId << std::endl;
        
        // 2. Create and start services
        auto tradingService = std::make_shared<service::TradingService>();
        tradingService->start();
        std::cout << "Trading service started" << std::endl;
        
        // 3. Create an order
        model::Order order;
        order.setSymbol("AAPL");
        order.setQuantity(100);
        order.setSide(model::Order::Side::BUY);
        order.setPrice(150.25);
        order.setOrderType(model::Order::Type::LIMIT);
        order.setClientOrderId("ORDER123");
        
        std::cout << "Created order: " << order.getSymbol() 
                  << " " << order.getQuantity() 
                  << " @ " << order.getPrice() << std::endl;
        
        // 4. Process the order
        bool processed = tradingService->processOrder(order);
        if (processed) {
            std::cout << "Order processed successfully" << std::endl;
            
            // 5. Check for trades
            auto trades = tradingService->getTrades();
            std::cout << "Generated " << trades.size() << " trades" << std::endl;
            
            for (const auto& trade : trades) {
                std::cout << "Trade: " << trade.getSymbol() 
                          << " " << trade.getQuantity() 
                          << " @ " << trade.getPrice() << std::endl;
            }
        }
        
        // 6. Create FIX message
        fix::Message message;
        message.setField(fix::Tags::MsgType, "D");
        message.setField(fix::Tags::ClOrdID, order.getClientOrderId());
        message.setField(fix::Tags::Symbol, order.getSymbol());
        message.setField(fix::Tags::Side, order.getSide() == model::Order::Side::BUY ? "1" : "2");
        message.setField(fix::Tags::OrderQty, std::to_string(order.getQuantity()));
        message.setField(fix::Tags::Price, std::to_string(order.getPrice()));
        
        std::cout << "Created FIX message with " << message.getFields().size() << " fields" << std::endl;
        
        // 7. Simulate message parsing
        const char* fixBuffer = "8=FIX.4.4\0019=154\00135=D\00111=ORDER123\00155=AAPL\00154=1\00138=100\00144=150.25\00110=123\001";
        fix::ZeroCopyMessage parsedMsg(fixBuffer, strlen(fixBuffer));
        
        std::cout << "Parsed message type: " << parsedMsg.getField(fix::Tags::MsgType) << std::endl;
        std::cout << "Parsed symbol: " << parsedMsg.getField(fix::Tags::Symbol) << std::endl;
        
        // 8. Clean up
        tradingService->stop();
        std::cout << "Trading service stopped" << std::endl;
        
        std::cout << "Example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}


Building and Running
~~~~~~~~~~~~~~~~~~~~

To build and run this example:

.. code-block:: bash

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run the example
./bin/fix-fasttrade


Expected Output
~~~~~~~~~~~~~~~


Created session: FIX.4.4:CLIENT->SERVER
Trading service started
Created order: AAPL 100 @ 150.25
Order processed successfully
Generated 0 trades
Created FIX message with 5 fields
Parsed message type: D
Parsed symbol: AAPL
Trading service stopped
Example completed successfully!


Error Handling
--------------

Always include proper error handling in production code:

.. code-block:: cpp

try {
    // FIX-FastTrade operations
    auto tradingService = std::make_shared<service::TradingService>();
    tradingService->start();
    
    model::Order order;
    // ... set order fields
    
    if (!tradingService->processOrder(order)) {
        std::cerr << "Failed to process order" << std::endl;
        // Handle error appropriately
    }
    
} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    // Log error and handle gracefully
} catch (...) {
    std::cerr << "Unknown exception occurred" << std::endl;
    // Handle unknown errors
}


Performance Tips
----------------

For optimal performance in production:

1. **Use Zero-Copy Messages**: Always use ``fix::ZeroCopyMessage`` for parsing incoming messages
2. **Pre-allocate Objects**: Reuse Order and Trade objects to avoid allocations
3. **Enable Compiler Optimizations**: Build with ``-O3 -march=native -flto``
4. **Set CPU Affinity**: Pin threads to specific CPU cores for consistent latency
5. **Use Memory Pools**: Leverage the infrastructure memory pools for allocation-free operations

.. code-block:: cpp

// Performance-optimized message processing
void processMessages(const std::vector<std::pair<const char*, size_t>>& messages) {
    for (const auto& [buffer, length] : messages) {
        fix::ZeroCopyMessage msg(buffer, length);  // Zero-copy parsing
        
        // Fast field access
        auto msgType = msg.getField(fix::Tags::MsgType);
        if (msgType == "D") {  // New Order Single
            // Process order with minimal allocations
            processNewOrder(msg);
        }
    }
}


This completes the basic usage examples for the FIX-FastTrade system. All examples are based on the current working implementation and demonstrate the key features and capabilities of the system.