Configuration Reference
=======================

This section provides a complete reference for all FIX-FastTrade configuration options.

Configuration File Format
--------------------------

FIX-FastTrade uses XML configuration files with the following structure:

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8"?>
    <fix>
        <session>
            <!-- Session configuration -->
        </session>
        <logging>
            <!-- Logging configuration -->
        </logging>
        <performance>
            <!-- Performance configuration -->
        </performance>
        <security>
            <!-- Security configuration -->
        </security>
    </fix>

Session Configuration
---------------------

Required Fields
~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 15 15 50

   * - Field
     - Type
     - Default
     - Description
   * - senderCompID
     - string
     - *required*
     - Unique identifier for your system
   * - targetCompID
     - string
     - *required*
     - Target system identifier
   * - host
     - string
     - *required*
     - FIX server hostname or IP address
   * - port
     - integer
     - *required*
     - FIX server port number

Optional Fields
~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 15 15 50

   * - Field
     - Type
     - Default
     - Description
   * - version
     - string
     - FIX.4.4
     - FIX protocol version
   * - heartBtInt
     - integer
     - 30
     - Heartbeat interval in seconds
   * - reconnectInterval
     - integer
     - 5
     - Reconnection interval in seconds
   * - maxReconnectAttempts
     - integer
     - 10
     - Maximum reconnection attempts

Logging Configuration
---------------------

Basic Logging Options
~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 15 15 50

   * - Field
     - Type
     - Default
     - Description
   * - level
     - string
     - INFO
     - Global log level (DEBUG, INFO, WARN, ERROR)
   * - console
     - boolean
     - false
     - Enable console logging
   * - file
     - string
     - logs/fix-fasttrade.log
     - Log file path

Performance Configuration
-------------------------

CPU and Threading
~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 15 15 50

   * - Field
     - Type
     - Default
     - Description
   * - cpuAffinity
     - boolean
     - false
     - Enable CPU affinity
   * - cpuMain
     - integer
     - 0
     - CPU core for main thread
   * - threadPriority
     - integer
     - 0
     - Thread priority (-20 to 20)

Configuration Examples
----------------------

Minimal Configuration
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8"?>
    <fix>
        <session>
            <senderCompID>TRADER_001</senderCompID>
            <targetCompID>EXCHANGE</targetCompID>
            <host>localhost</host>
            <port>9878</port>
        </session>
    </fix>

Production Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8"?>
    <fix>
        <session>
            <senderCompID>PROD_TRADER_001</senderCompID>
            <targetCompID>EXCHANGE_PROD</targetCompID>
            <host>fix.exchange.com</host>
            <port>9878</port>
            <heartBtInt>30</heartBtInt>
            <validateMessages>true</validateMessages>
        </session>
        
        <logging>
            <level>WARN</level>
            <file>/var/log/fix-fasttrade/fix-fasttrade.log</file>
        </logging>
        
        <performance>
            <cpuAffinity>true</cpuAffinity>
            <cpuMain>0</cpuMain>
            <threadPriority>-15</threadPriority>
        </performance>
    </fix>

For more configuration examples, see the ``config/examples/`` directory in the source distribution.