# FIX-FastTrade User Guide

Welcome to the FIX-FastTrade User Guide! This document will provide you with a comprehensive overview of how to use and interact with the FIX-FastTrade system.

## Table of Contents

1. [Introduction](#introduction)
2. [System Requirements](#system-requirements)
3. [Installation](#installation)
4. [Configuration](#configuration)
   - [FIX Session Configuration](#fix-session-configuration)
   - [Connectivity Configuration](#connectivity-configuration)
   - [Logging Configuration](#logging-configuration)
5. [Starting FIX-FastTrade](#starting-fix-fasttrade)
6. [Placing Orders](#placing-orders)
   - [New Order Single](#new-order-single)
   - [Order Cancel Request](#order-cancel-request)
   - [Order Cancel/Replace Request](#order-cancelreplace-request)
7. [Receiving Execution Reports](#receiving-execution-reports)
8. [Market Data Subscription](#market-data-subscription)
9. [Risk Management](#risk-management)
10. [Troubleshooting](#troubleshooting)
11. [FAQ](#faq)
12. [Support](#support)

## Introduction

FIX-FastTrade is a high-performance electronic trading system that leverages the Financial Information eXchange (FIX) protocol for fast and reliable communication between trading parties. It provides a robust and efficient platform for executing trades with low latency and high throughput.

## System Requirements

To run FIX-FastTrade, ensure that your system meets the following requirements:

- Operating System: Windows, macOS, or Linux
- C++17 compiler
- CMake (version 3.10 or higher)
- Boost library (version 1.71.0 or higher)

## Installation

Follow these steps to install FIX-FastTrade on your system:

1. Clone the FIX-FastTrade repository from GitHub.
2. Navigate to the project directory.
3. Create a build directory and navigate to it.
4. Generate the build files using CMake.
5. Build the project using the appropriate build system (e.g., make, Visual Studio).
6. The FIX-FastTrade executable will be generated in the `bin` directory.

Refer to the [README](../README.md) file for detailed installation instructions.

## Configuration

FIX-FastTrade can be configured using the configuration files located in the `config` directory. The main configuration file is `config.txt`, which contains settings for the FIX session, connectivity, and logging.

### FIX Session Configuration

- `BeginString`: Specify the FIX protocol version (e.g., FIX.4.4).
- `SenderCompID`: Set the sender's company ID.
- `TargetCompID`: Set the target's company ID.
- `HeartBtInt`: Specify the heartbeat interval in seconds.

### Connectivity Configuration

- `SocketConnectHost`: Set the IP address or hostname of the FIX server.
- `SocketConnectPort`: Specify the port number to connect to the FIX server.
- `SocketConnectTimeout`: Set the connection timeout in seconds.

### Logging Configuration

- `LogFile`: Specify the path to the log file.
- `LogLevel`: Set the logging level (e.g., DEBUG, INFO, WARNING, ERROR).
- `LogTimestampPrecision`: Specify the timestamp precision for logging.

Modify the configuration files according to your specific requirements and trading environment.

## Starting FIX-FastTrade

To start FIX-FastTrade, follow these steps:

1. Open a terminal or command prompt.
2. Navigate to the FIX-FastTrade installation directory.
3. Run the `fix-fasttrade` executable.

FIX-FastTrade will start and establish a connection to the specified FIX server.

## Placing Orders

FIX-FastTrade supports various types of orders, including:

### New Order Single

To place a new order, send a New Order Single (35=D) message with the following fields:

- `ClOrdID` (11): Unique identifier for the order.
- `Symbol` (55): Ticker symbol or instrument identifier.
- `Side` (54): Buy (1) or Sell (2).
- `OrderQty` (38): Quantity of the order.
- `OrdType` (40): Order type (e.g., Market, Limit, Stop).
- `Price` (44): Order price (required for Limit and Stop orders).
- `TimeInForce` (59): Time in force for the order (e.g., Day, GTC, IOC).

Example:
```
8=FIX.4.4|9=145|35=D|49=SENDER|56=TARGET|34=1|52=20230525-10:30:00|11=ORDER123|55=AAPL|54=1|38=100|40=2|44=150.00|59=0|10=123|
```

### Order Cancel Request

To cancel an existing order, send an Order Cancel Request (35=F) message with the following fields:

- `OrigClOrdID` (41): The `ClOrdID` of the order to be canceled.
- `ClOrdID` (11): Unique identifier for the cancel request.
- `Symbol` (55): Ticker symbol or instrument identifier.
- `Side` (54): Buy (1) or Sell (2).

Example:
```
8=FIX.4.4|9=145|35=F|49=SENDER|56=TARGET|34=2|52=20230525-10:31:00|41=ORDER123|11=CANCEL123|55=AAPL|54=1|10=234|
```

### Order Cancel/Replace Request

To modify an existing order, send an Order Cancel/Replace Request (35=G) message with the following fields:

- `OrigClOrdID` (41): The `ClOrdID` of the order to be modified.
- `ClOrdID` (11): Unique identifier for the cancel/replace request.
- `Symbol` (55): Ticker symbol or instrument identifier.
- `Side` (54): Buy (1) or Sell (2).
- `OrderQty` (38): New quantity of the order.
- `OrdType` (40): New order type (e.g., Market, Limit, Stop).
- `Price` (44): New order price (required for Limit and Stop orders).

Example:
```
8=FIX.4.4|9=145|35=G|49=SENDER|56=TARGET|34=3|52=20230525-10:32:00|41=ORDER123|11=REPLACE123|55=AAPL|54=1|38=50|40=2|44=155.00|10=345|
```

## Receiving Execution Reports

FIX-FastTrade will send Execution Report (35=8) messages to provide updates on the status of orders. The following fields are commonly included in the Execution Report:

- `OrderID` (37): Unique identifier assigned by the exchange or broker.
- `ExecID` (17): Unique identifier for the execution report.
- `ExecType` (150): Execution type (e.g., New, Canceled, Replaced, Rejected).
- `OrdStatus` (39): Order status (e.g., New, Partially Filled, Filled, Canceled, Rejected).
- `Symbol` (55): Ticker symbol or instrument identifier.
- `Side` (54): Buy (1) or Sell (2).
- `OrderQty` (38): Quantity of the order.
- `CumQty` (14): Cumulative quantity filled.
- `AvgPx` (6): Average price of the fills.
- `LeavesQty` (151): Remaining quantity of the order.

Example:
```
8=FIX.4.4|9=145|35=8|49=TARGET|56=SENDER|34=4|52=20230525-10:33:00|37=ORDERID123|17=EXECID123|150=0|39=0|55=AAPL|54=1|38=100|14=0|6=0|151=100|10=456|
```

## Market Data Subscription

To subscribe to market data, send a Market Data Request (35=V) message with the following fields:

- `MDReqID` (262): Unique identifier for the market data request.
- `SubscriptionRequestType` (263): Subscription request type (e.g., Snapshot, Subscribe, Unsubscribe).
- `MarketDepth` (264): Market depth requested (e.g., Top of Book, Full Book).
- `MDUpdateType` (265): Market data update type (e.g., Full Refresh, Incremental Refresh).
- `NoMDEntryTypes` (267): Number of market data entry types requested.
- `MDEntryType` (269): Market data entry type (e.g., Bid, Offer, Trade).
- `NoRelatedSym` (146): Number of symbols requested.
- `Symbol` (55): Ticker symbol or instrument identifier.

Example:
```
8=FIX.4.4|9=145|35=V|49=SENDER|56=TARGET|34=5|52=20230525-10:34:00|262=MDREQ123|263=1|264=0|265=0|267=2|269=0|269=1|146=1|55=AAPL|10=567|
```

FIX-FastTrade will send Market Data Snapshot/Full Refresh (35=W) and Market Data Incremental Refresh (35=X) messages to provide the requested market data.

## Risk Management

FIX-FastTrade provides risk management features to monitor and control trading activities. It allows setting up risk limits based on various parameters, such as:

- Maximum order quantity
- Maximum order value
- Maximum position size
- Maximum daily loss

Risk limits can be configured in the `risk_config.txt` file. If a risk limit is breached, FIX-FastTrade will reject the order and send an appropriate Execution Report with a rejection reason.

## Troubleshooting

If you encounter any issues while using FIX-FastTrade, consider the following troubleshooting steps:

- Verify that the FIX-FastTrade configuration files are properly set up and match the requirements of your trading environment.
- Check the log files for any error messages or stacktraces that can help identify the issue.
- Ensure that the FIX server is running and accessible from the FIX-FastTrade instance.
- Verify that the FIX messages being sent are properly formatted and contain valid values for the required fields.
- Consult the FIX-FastTrade documentation and FAQ section for common issues and their resolutions.

If the issue persists, reach out to the FIX-FastTrade support team for further assistance.

## FAQ

1. **Q: How do I configure FIX-FastTrade to connect to my FIX server?**
   A: Modify the connectivity settings in the `config.txt` file, specifically the `SocketConnectHost` and `SocketConnectPort` parameters, to match your FIX server's IP address and port number.

2. **Q: Can FIX-FastTrade support multiple FIX sessions?**
   A: Yes, FIX-FastTrade can be configured to support multiple FIX sessions. Each session requires its own configuration file and can be started separately.

3. **Q: How do I integrate FIX-FastTrade with my existing trading systems?**
   A: FIX-FastTrade provides a standard FIX interface, which allows seamless integration with any trading system that supports the FIX protocol. You can configure your trading system to send and receive FIX messages to/from FIX-FastTrade.

4. **Q: What FIX protocol versions are supported by FIX-FastTrade?**
   A: FIX-FastTrade supports various FIX protocol versions, including FIX 4.2, FIX 4.4, and FIX 5.0. The specific version can be configured in the `BeginString` parameter in the `config.txt` file.

## Support

For any questions, issues, or support requirements, please contact the FIX-FastTrade support team at [support@fix-fasttrade.com](mailto:support@fix-fasttrade.com). We are available 24/7 to assist you with your FIX-FastTrade installation and usage.

You can also visit our website at [www.fix-fasttrade.com](https://fix-fasttrade.readthedocs.io/) for additional resources, documentation, and updates.

---