API Reference
=============

This section provides comprehensive API documentation for FIX-FastTrade components.

Overview
--------

FIX-FastTrade provides a clean, modern C++17 API organized into several key modules:

* **FIX Protocol API** - Core FIX protocol implementation
* **Model API** - Trading data models (Orders, Trades)
* **Service API** - Business logic services

API Modules
-----------

.. toctree::
   :maxdepth: 2

   fix-api
   model-api
   service-api

Quick API Reference
-------------------

Core Classes
~~~~~~~~~~~~

**FIX Protocol**

* ``fix::Session`` - FIX session management
* ``fix::Message`` - FIX message handling
* ``fix::MessageParser`` - Message parsing utilities
* ``fix::Application`` - FIX application interface

**Trading Models**

* ``model::Order`` - Order representation and lifecycle
* ``model::Trade`` - Trade execution and reporting

**Services**

* ``service::FixService`` - FIX protocol service layer
* ``service::TradingService`` - Core trading logic service

Common Usage Patterns
---------------------

Session Management
~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include <fix/Session.h>
   #include <fix/Config.h>
   
   // Create session configuration
   fix::Config config("config/fix-config.xml");
   fix::Session::SessionConfig sessionConfig;
   
   // Initialize session
   fix::Session session(config, sessionConfig, *application);
   
   // Connect and start session
   session.connect();

Message Handling
~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include <fix/Message.h>
   #include <fix/MessageParser.h>
   
   // Parse incoming FIX message
   std::string fixMessage = "8=FIX.4.4|9=148|35=D|...";
   fix::Message message = fix::MessageParser::parse(fixMessage);
   
   // Access message fields
   std::string msgType = message.getField(35);
   std::string symbol = message.getField(55);

Order Processing
~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include <model/Order.h>
   #include <service/TradingService.h>
   
   // Create new order
   model::Order order(
       "ORDER_001",           // Order ID
       "AAPL",               // Symbol
       150.50,               // Price
       1000,                 // Quantity
       model::Order::Side::Buy,
       model::Order::OrderType::Limit
   );
   
   // Process order through trading service
   service::TradingService tradingService;
   tradingService.start();
   bool success = tradingService.processOrder(order);

Error Handling
~~~~~~~~~~~~~~

.. code-block:: cpp

   #include <stdexcept>
   
   try {
       fix::Session session(config, sessionConfig, *application);
       session.connect();
   }
   catch (const std::runtime_error& e) {
       // Handle connection errors
       std::cerr << "Connection failed: " << e.what() << std::endl;
   }
   catch (const std::invalid_argument& e) {
       // Handle configuration errors
       std::cerr << "Configuration error: " << e.what() << std::endl;
   }

API Design Principles
---------------------

The FIX-FastTrade API follows these design principles:

**Performance First**
  All APIs are designed for minimal latency and maximum throughput

**Type Safety**
  Strong typing with C++17 features to prevent runtime errors

**RAII Design**
  Automatic resource management with smart pointers and RAII

**Exception Safety**
  Comprehensive error handling with meaningful exception types

**Thread Safety**
  Lock-free design where possible, clear thread safety guarantees

**Modern C++**
  Leverages C++17 features for clean, efficient code

API Conventions
---------------

Naming Conventions
~~~~~~~~~~~~~~~~~~

* **Classes**: PascalCase (``FixService``, ``TradingService``)
* **Methods**: camelCase (``processOrder``, ``getField``)
* **Constants**: UPPER_SNAKE_CASE (``MAX_MESSAGE_SIZE``)
* **Namespaces**: lowercase (``fix``, ``model``, ``service``)

Memory Management
~~~~~~~~~~~~~~~~~

* Use smart pointers (``std::unique_ptr``, ``std::shared_ptr``)
* RAII for resource management
* Custom allocators for performance-critical paths
* Zero-copy operations where possible

Error Handling
~~~~~~~~~~~~~~

* Use exceptions for error conditions
* Provide meaningful error messages
* Use specific exception types
* Document exception specifications

Thread Safety
~~~~~~~~~~~~~

* Clearly document thread safety guarantees
* Use atomic operations for shared state
* Prefer lock-free algorithms
* Provide thread-safe alternatives where needed

API Versioning
--------------

FIX-FastTrade uses semantic versioning for API compatibility:

* **Major version** (2.x.x): Breaking API changes
* **Minor version** (x.1.x): New features, backward compatible
* **Patch version** (x.x.1): Bug fixes, backward compatible

Deprecated APIs are marked with ``[[deprecated]]`` and removed in the next major version.

Getting Help
------------

For API questions and support:

* Check the :doc:``../troubleshooting/index`` section
* Review the example code in ``examples/``
* Consult the inline documentation in header files
* Open an issue on GitHub for bugs or feature requests