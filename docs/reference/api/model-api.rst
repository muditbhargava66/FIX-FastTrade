# Model API

This document provides an overview of the Model API provided by the FIX-FastTrade system. The Model API defines the data models and structures used throughout the system.

Order
-----

The ``Order`` class represents an order in the trading system with comprehensive order management capabilities.

Enumerations
~~~~~~~~~~~~

.. code-block:: cpp

enum class Side { Buy, Sell };
enum class OrderType { Market, Limit, Stop, StopLimit };
enum class OrderStatus { New, Accepted, Rejected, PartiallyFilled, Filled, Cancelled };


Constructors
~~~~~~~~~~~~

.. code-block:: cpp

// Default constructor for pool allocation
Order();

// Full constructor
Order(const std::string& id, const std::string& symbol, double price, int quantity,
      Side side, OrderType type);


Accessor Methods
~~~~~~~~~~~~~~~~

.. code-block:: cpp

const std::string& getId() const;
const std::string& getSymbol() const;
double getPrice() const;
int getQuantity() const;
Side getSide() const;
OrderType getType() const;
OrderStatus getStatus() const;


Mutator Methods
~~~~~~~~~~~~~~~

.. code-block:: cpp

void setQuantity(int quantity);
void setStatus(OrderStatus status);


Status Check Methods
~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

bool isFilled() const;
bool isPartiallyFilled() const;
bool isCancelled() const;


Trade
-----

The ``Trade`` class represents a trade that occurs as a result of order matching.

Enumerations
~~~~~~~~~~~~

.. code-block:: cpp

enum class Side { Buy, Sell };


Constructor
~~~~~~~~~~~

.. code-block:: cpp

Trade(const std::string& id, const std::string& orderId, const std::string& symbol,
      double price, int quantity, Side side, std::chrono::system_clock::time_point timestamp);


Accessor Methods
~~~~~~~~~~~~~~~~

.. code-block:: cpp

const std::string& getId() const;
const std::string& getOrderId() const;
const std::string& getSymbol() const;
double getPrice() const;
int getQuantity() const;
Side getSide() const;
std::chrono::system_clock::time_point getTimestamp() const;


Properties
~~~~~~~~~~

* ``id``: The unique identifier of the trade
* ``orderId``: The identifier of the order associated with the trade
* ``symbol``: The trading symbol or instrument identifier
* ``price``: The execution price of the trade
* ``quantity``: The quantity of the trade
* ``side``: The side of the trade (buy or sell)
* ``timestamp``: The timestamp of when the trade occurred (using ``std::chrono::system_clock::time_point``)

Usage Examples
--------------

Creating an Order
~~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "model/Order.h"

// Create a limit buy order
model::Order order("ORD001", "AAPL", 150.00, 100, 
                   model::Order::Side::Buy, 
                   model::Order::OrderType::Limit);

// Check order properties
std::cout << "Order ID: " << order.getId() << std::endl;
std::cout << "Symbol: " << order.getSymbol() << std::endl;
std::cout << "Price: " << order.getPrice() << std::endl;
std::cout << "Quantity: " << order.getQuantity() << std::endl;

// Update order status
order.setStatus(model::Order::OrderStatus::Accepted);

// Check order status
if (order.isFilled()) {
    std::cout << "Order is filled" << std::endl;
} else if (order.isPartiallyFilled()) {
    std::cout << "Order is partially filled" << std::endl;
}


Creating a Trade
~~~~~~~~~~~~~~~~

.. code-block:: cpp

#include "model/Trade.h"
#include <chrono>

// Create a trade
auto timestamp = std::chrono::system_clock::now();
model::Trade trade("TRD001", "ORD001", "AAPL", 149.95, 50, 
                   model::Trade::Side::Buy, timestamp);

// Access trade properties
std::cout << "Trade ID: " << trade.getId() << std::endl;
std::cout << "Order ID: " << trade.getOrderId() << std::endl;
std::cout << "Symbol: " << trade.getSymbol() << std::endl;
std::cout << "Price: " << trade.getPrice() << std::endl;
std::cout << "Quantity: " << trade.getQuantity() << std::endl;


Design Notes
------------

Memory Efficiency
~~~~~~~~~~~~~~~~~

The ``Order`` class includes a default constructor to support efficient memory pool allocation, which is important for high-frequency trading scenarios.

Type Safety
~~~~~~~~~~~

Both ``Order`` and ``Trade`` classes use strongly-typed enumerations (``enum class``) to prevent type confusion and improve code safety.

Time Handling
~~~~~~~~~~~~~

The ``Trade`` class uses ``std::chrono::system_clock::time_point`` for precise timestamp handling, which is crucial for trade reporting and audit trails.

For more detailed information on the Model API and its usage, please refer to the FIX-FastTrade documentation and the source code documentation.

