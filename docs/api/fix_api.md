# FIX API Documentation

This document provides an overview of the FIX API provided by the FIX-FastTrade system. The FIX API allows external trading systems to integrate with FIX-FastTrade using the Financial Information eXchange (FIX) protocol.

## Session Management

The FIX API supports session management functionality to establish and maintain connections with external trading systems.

### Session Initialization

To initialize a FIX session, use the following function:

```cpp
bool initializeSession(const SessionSettings& settings, Application& application);
```

- `settings`: The session settings object containing configuration parameters for the FIX session.
- `application`: The application object implementing the `Application` interface to handle session events and messages.

### Session Termination

To terminate a FIX session, use the following function:

```cpp
void terminateSession(const SessionID& sessionID);
```

- `sessionID`: The unique identifier of the FIX session to terminate.

## Message Handling

The FIX API provides functions to send and receive FIX messages.

### Sending Messages

To send a FIX message, use the following function:

```cpp
bool sendMessage(const Message& message, const SessionID& sessionID);
```

- `message`: The FIX message object to send.
- `sessionID`: The unique identifier of the FIX session to send the message to.

### Receiving Messages

To receive FIX messages, implement the following callback function in your `Application` class:

```cpp
void fromApp(const Message& message, const SessionID& sessionID);
```

- `message`: The received FIX message object.
- `sessionID`: The unique identifier of the FIX session that received the message.

## Order Management

The FIX API provides functions for managing orders.

### New Order Single

To send a new order, use the following function:

```cpp
bool sendNewOrderSingle(const NewOrderSingle& order, const SessionID& sessionID);
```

- `order`: The `NewOrderSingle` object representing the new order.
- `sessionID`: The unique identifier of the FIX session to send the order to.

### Order Cancel Request

To send an order cancel request, use the following function:

```cpp
bool sendOrderCancelRequest(const OrderCancelRequest& request, const SessionID& sessionID);
```

- `request`: The `OrderCancelRequest` object representing the cancel request.
- `sessionID`: The unique identifier of the FIX session to send the request to.

### Order Cancel/Replace Request

To send an order cancel/replace request, use the following function:

```cpp
bool sendOrderCancelReplaceRequest(const OrderCancelReplaceRequest& request, const SessionID& sessionID);
```

- `request`: The `OrderCancelReplaceRequest` object representing the cancel/replace request.
- `sessionID`: The unique identifier of the FIX session to send the request to.

## Market Data

The FIX API provides functions for subscribing to and receiving market data.

### Market Data Request

To send a market data request, use the following function:

```cpp
bool sendMarketDataRequest(const MarketDataRequest& request, const SessionID& sessionID);
```

- `request`: The `MarketDataRequest` object representing the market data request.
- `sessionID`: The unique identifier of the FIX session to send the request to.

### Market Data Snapshot/Full Refresh

To receive market data snapshots or full refreshes, implement the following callback function in your `Application` class:

```cpp
void fromApp(const MarketDataSnapshotFullRefresh& message, const SessionID& sessionID);
```

- `message`: The received `MarketDataSnapshotFullRefresh` message object.
- `sessionID`: The unique identifier of the FIX session that received the message.

### Market Data Incremental Refresh

To receive market data incremental refreshes, implement the following callback function in your `Application` class:

```cpp
void fromApp(const MarketDataIncrementalRefresh& message, const SessionID& sessionID);
```

- `message`: The received `MarketDataIncrementalRefresh` message object.
- `sessionID`: The unique identifier of the FIX session that received the message.

For more detailed information on the FIX API and its usage, please refer to the FIX protocol specification and the FIX-FastTrade documentation.