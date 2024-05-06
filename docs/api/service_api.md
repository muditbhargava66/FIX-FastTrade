# Service API Documentation

This document provides an overview of the Service API provided by the FIX-FastTrade system. The Service API defines the high-level services and operations available for interacting with the trading system.

## Trading Service

The Trading Service provides functionality for managing orders and executing trades.

### Methods

- `placeOrder(order)`: Places a new order in the trading system.
  - `order`: The `Order` object representing the order to be placed.
- `cancelOrder(orderId)`: Cancels an existing order.
  - `orderId`: The unique identifier of the order to be canceled.
- `modifyOrder(order)`: Modifies an existing order.
  - `order`: The `Order` object representing the modified order.
- `getOrder(orderId)`: Retrieves the details of an order.
  - `orderId`: The unique identifier of the order to retrieve.
- `getOpenOrders()`: Retrieves a list of all open orders.
- `getExecutions(orderId)`: Retrieves the executions (trades) associated with an order.
  - `orderId`: The unique identifier of the order to retrieve executions for.

## Market Data Service

The Market Data Service provides functionality for retrieving and subscribing to market data.

### Methods

- `getMarketData(symbol)`: Retrieves the current market data for a given symbol.
  - `symbol`: The trading symbol or instrument identifier.
- `subscribeMarketData(symbol)`: Subscribes to real-time market data updates for a given symbol.
  - `symbol`: The trading symbol or instrument identifier.
- `unsubscribeMarketData(symbol)`: Unsubscribes from real-time market data updates for a given symbol.
  - `symbol`: The trading symbol or instrument identifier.

## Position Service

The Position Service provides functionality for managing positions in a trading account.

### Methods

- `getPositions()`: Retrieves a list of all positions in the trading account.
- `getPosition(symbol)`: Retrieves the position for a given symbol.
  - `symbol`: The trading symbol or instrument identifier.
- `closePosition(symbol)`: Closes the position for a given symbol.
  - `symbol`: The trading symbol or instrument identifier.

## Risk Management Service

The Risk Management Service provides functionality for managing risk and enforcing trading limits.

### Methods

- `setRiskLimit(limitType, limit)`: Sets a risk limit for a given limit type.
  - `limitType`: The type of risk limit (e.g., maximum order quantity, maximum position size).
  - `limit`: The value of the risk limit.
- `getRiskLimit(limitType)`: Retrieves the current risk limit for a given limit type.
  - `limitType`: The type of risk limit.
- `checkRiskLimit(order)`: Checks if an order violates any risk limits.
  - `order`: The `Order` object representing the order to check.

## Reporting Service

The Reporting Service provides functionality for generating reports and retrieving historical data.

### Methods

- `getTradeHistory(startDate, endDate)`: Retrieves the trade history for a given date range.
  - `startDate`: The start date of the date range.
  - `endDate`: The end date of the date range.
- `getOrderHistory(startDate, endDate)`: Retrieves the order history for a given date range.
  - `startDate`: The start date of the date range.
  - `endDate`: The end date of the date range.
- `generateDailyReport(date)`: Generates a daily report for a given date.
  - `date`: The date for which to generate the report.

For more detailed information on the Service API and its usage, please refer to the FIX-FastTrade documentation and the source code documentation.