Configuration Guide
===================

FIX-FastTrade uses XML-based configuration files to manage system settings, FIX sessions, and performance parameters.

Configuration Files
-------------------

Main Configuration Files
~~~~~~~~~~~~~~~~~~~~~~~~

* **fix-config.xml** - Main FIX session configuration
* **system/performance.json** - Performance tuning parameters
* **examples/** - Working configuration templates

Configuration Structure
~~~~~~~~~~~~~~~~~~~~~~~

The main configuration file follows this structure:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>SENDER</senderCompID>
           <targetCompID>TARGET</targetCompID>
           <host>localhost</host>
           <port>9878</port>
           <version>FIX.4.4</version>
           <heartBtInt>30</heartBtInt>
       </session>
       <logging>
           <level>INFO</level>
           <file>logs/fix-fasttrade.log</file>
       </logging>
       <performance>
           <cpuAffinity>true</cpuAffinity>
           <memoryLock>true</memoryLock>
           <priority>-10</priority>
       </performance>
   </fix>

Session Configuration
---------------------

Basic Session Settings
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <session>
       <!-- Required: Unique identifier for your system -->
       <senderCompID>YOUR_COMPANY_ID</senderCompID>
       
       <!-- Required: Target system identifier -->
       <targetCompID>EXCHANGE_ID</targetCompID>
       
       <!-- Required: FIX server connection details -->
       <host>fix.exchange.com</host>
       <port>9878</port>
       
       <!-- Optional: FIX protocol version (default: FIX.4.4) -->
       <version>FIX.4.4</version>
       
       <!-- Optional: Heartbeat interval in seconds (default: 30) -->
       <heartBtInt>30</heartBtInt>
   </session>

Advanced Session Settings
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <session>
       <!-- Connection settings -->
       <reconnectInterval>5</reconnectInterval>
       <maxReconnectAttempts>10</maxReconnectAttempts>
       <connectTimeout>30</connectTimeout>
       
       <!-- Message settings -->
       <maxMessageSize>8192</maxMessageSize>
       <validateMessages>true</validateMessages>
       <logIncomingMessages>true</logIncomingMessages>
       <logOutgoingMessages>true</logOutgoingMessages>
       
       <!-- Sequence number management -->
       <resetSeqNumFlag>true</resetSeqNumFlag>
       <nextSenderMsgSeqNum>1</nextSenderMsgSeqNum>
       <nextTargetMsgSeqNum>1</nextTargetMsgSeqNum>
   </session>

Performance Configuration
-------------------------

CPU Affinity Settings
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <performance>
       <cpuAffinity>true</cpuAffinity>
       <cpuMain>0</cpuMain>
       <cpuFix>1</cpuFix>
       <cpuOrder>2</cpuOrder>
       <cpuMarket>3</cpuMarket>
   </performance>

Memory Management
~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <performance>
       <memoryLock>true</memoryLock>
       <memoryPoolSize>1048576</memoryPoolSize>
       <preallocateMemory>true</preallocateMemory>
   </performance>

Threading Configuration
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <performance>
       <threadPriority>-10</threadPriority>
       <useRealTimeScheduling>true</useRealTimeScheduling>
       <threadStackSize>8388608</threadStackSize>
   </performance>

Logging Configuration
---------------------

Basic Logging
~~~~~~~~~~~~~

.. code-block:: xml

   <logging>
       <level>INFO</level>
       <file>logs/fix-fasttrade.log</file>
       <maxFileSize>100MB</maxFileSize>
       <maxFiles>10</maxFiles>
   </logging>

Advanced Logging
~~~~~~~~~~~~~~~~

.. code-block:: xml

   <logging>
       <level>DEBUG</level>
       <console>true</console>
       <file>logs/fix-fasttrade.log</file>
       <rotateDaily>true</rotateDaily>
       <timestampFormat>%Y-%m-%d %H:%M:%S.%f</timestampFormat>
       
       <!-- Component-specific logging -->
       <components>
           <fix>DEBUG</fix>
           <trading>INFO</trading>
           <network>WARN</network>
       </components>
   </logging>

Environment-Specific Configurations
-----------------------------------

Development Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>DEV_TRADER</senderCompID>
           <targetCompID>DEV_EXCHANGE</targetCompID>
           <host>localhost</host>
           <port>9878</port>
           <heartBtInt>60</heartBtInt>
       </session>
       <logging>
           <level>DEBUG</level>
           <console>true</console>
       </logging>
       <performance>
           <cpuAffinity>false</cpuAffinity>
           <memoryLock>false</memoryLock>
       </performance>
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
           <level>INFO</level>
           <file>/var/log/fix-fasttrade/fix-fasttrade.log</file>
           <rotateDaily>true</rotateDaily>
       </logging>
       <performance>
           <cpuAffinity>true</cpuAffinity>
           <cpuMain>0</cpuMain>
           <cpuFix>1</cpuFix>
           <cpuOrder>2</cpuOrder>
           <memoryLock>true</memoryLock>
           <threadPriority>-10</threadPriority>
       </performance>
   </fix>

Configuration Validation
------------------------

Validate Configuration
~~~~~~~~~~~~~~~~~~~~~~

Before starting the system, validate your configuration:

.. code-block:: bash

   # Validate XML syntax
   xmllint --noout config/fix-config.xml
   
   # Test configuration with dry-run
   ./bin/fix-fasttrade --config config/fix-config.xml --validate

Common Configuration Issues
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Invalid XML Syntax**

.. code-block:: text

   Error: XML parsing failed at line 15: mismatched tag

Solution: Use xmllint to check syntax:

.. code-block:: bash

   xmllint --noout config/fix-config.xml

**Missing Required Fields**

.. code-block:: text

   Error: Required field 'senderCompID' not found

Solution: Ensure all required fields are present in the session configuration.

**Port Already in Use**

.. code-block:: text

   Error: Cannot bind to port 9878: Address already in use

Solution: Change the port number or stop the conflicting service:

.. code-block:: bash

   # Find process using the port
   lsof -i :9878
   
   # Kill the process or use a different port
   <port>9879</port>

Configuration Examples
----------------------

The ``config/examples/`` directory contains working configuration templates:

* **basic-config.xml** - Minimal working configuration
* **high-performance-config.xml** - Optimized for low latency
* **development-config.xml** - Development environment settings
* **production-config.xml** - Production deployment configuration

Command Line Configuration
--------------------------

Many configuration options can be overridden via command line:

.. code-block:: bash

   ./bin/fix-fasttrade \
     --config config/fix-config.xml \
     --cpu-main 0 \
     --cpu-fix 1 \
     --cpu-order 2 \
     --memory-lock \
     --priority -10 \
     --stats-interval 30 \
     --log-level DEBUG

For a complete list of command line options:

.. code-block:: bash

   ./bin/fix-fasttrade --help