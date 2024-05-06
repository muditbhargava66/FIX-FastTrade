# Model API Documentation

This document provides an overview of the Model API provided by the FIX-FastTrade system. The Model API defines the data models and structures used throughout the system.

## Order

The `Order` class represents an order in the trading system.

### Properties

- `id`: The unique identifier of the order.
- `symbol`: The trading symbol or instrument identifier.
- `price`: The price of the order.
- `quantity`: The quantity of the order.
- `side`: The side of the order (buy or sell).
- `type`: The type of the order (market, limit, stop, etc.).
- `status`: The current status of the order (new, partially filled, filled, canceled, etc.).

### Methods

- `getters`: Accessor methods for retrieving the values of the order properties.
- `setters`: Mutator methods for modifying the values of the order properties.
- `isFilled()`: Checks if the order is completely filled.
- `isPartiallyFilled()`: Checks if the order is partially filled.
- `isCanceled()`: Checks if the order is canceled.

## Trade

The `Trade` class represents a trade that occurs as a result of order matching.

### Properties

- `id`: The unique identifier of the trade.
- `orderId`: The identifier of the order associated with the trade.
- `symbol`: The trading symbol or instrument identifier.
- `price`: The execution price of the trade.
- `quantity`: The quantity of the trade.
- `side`: The side of the trade (buy or sell).
- `timestamp`: The timestamp of when the trade occurred.

### Methods

- `getters`: Accessor methods for retrieving the values of the trade properties.

## Market Data

The `MarketData` class represents market data information.

### Properties

- `symbol`: The trading symbol or instrument identifier.
- `bid`: The best bid price.
- `ask`: The best ask price.
- `bidSize`: The size or quantity available at the best bid price.
- `askSize`: The size or quantity available at the best ask price.
- `timestamp`: The timestamp of the market data update.

### Methods

- `getters`: Accessor methods for retrieving the values of the market data properties.

## Position

The `Position` class represents a position in a trading account.

### Properties

- `symbol`: The trading symbol or instrument identifier.
- `quantity`: The quantity of the position (positive for long, negative for short).
- `averagePrice`: The average price of the position.
- `marketValue`: The current market value of the position.

### Methods

- `getters`: Accessor methods for retrieving the values of the position properties.
- `updatePosition()`: Updates the position based on new trades or market data.
- `closePosition()`: Closes the position by generating the necessary orders.

For more detailed information on the Model API and its usage, please refer to the FIX-FastTrade documentation and the source code documentation.