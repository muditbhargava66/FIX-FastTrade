Advanced Configuration Guide
============================

This guide covers advanced configuration topics for FIX-FastTrade, including performance tuning, security settings, and production deployment configurations.

Performance Tuning
------------------

CPU Affinity Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

For optimal performance, bind different components to specific CPU cores:

.. code-block:: xml

   <performance>
       <cpuAffinity>true</cpuAffinity>
       <cpuMain>0</cpuMain>        <!-- Main application thread -->
       <cpuFix>1</cpuFix>          <!-- FIX protocol processing -->
       <cpuOrder>2</cpuOrder>      <!-- Order processing -->
       <cpuMarket>3</cpuMarket>    <!-- Market data processing -->
   </performance>

Command line equivalent:

.. code-block:: bash

   ./bin/fix-fasttrade \
     --config config/fix-config.xml \
     --cpu-main 0 --cpu-fix 1 --cpu-order 2 --cpu-market 3

Memory Optimization
~~~~~~~~~~~~~~~~~~~

Configure memory settings for low-latency operation:

.. code-block:: xml

   <performance>
       <!-- Lock memory pages to prevent swapping -->
       <memoryLock>true</memoryLock>
       
       <!-- Pre-allocate memory pools -->
       <memoryPoolSize>1048576</memoryPoolSize>
       <preallocateMemory>true</preallocateMemory>
       
       <!-- Message buffer settings -->
       <messageBufferSize>8192</messageBufferSize>
       <maxConcurrentMessages>10000</maxConcurrentMessages>
   </performance>

Thread Priority Settings
~~~~~~~~~~~~~~~~~~~~~~~~

Set real-time thread priorities for critical components:

.. code-block:: xml

   <performance>
       <!-- Set thread priority (-20 to 20, lower is higher priority) -->
       <threadPriority>-10</threadPriority>
       
       <!-- Use real-time scheduling (requires root privileges) -->
       <useRealTimeScheduling>true</useRealTimeScheduling>
       
       <!-- Thread stack size in bytes -->
       <threadStackSize>8388608</threadStackSize>
   </performance>

Network Configuration
---------------------

Connection Settings
~~~~~~~~~~~~~~~~~~~

Configure network parameters for optimal connectivity:

.. code-block:: xml

   <session>
       <!-- Connection timeouts -->
       <connectTimeout>30</connectTimeout>
       <reconnectInterval>5</reconnectInterval>
       <maxReconnectAttempts>10</maxReconnectAttempts>
       
       <!-- TCP settings -->
       <tcpNoDelay>true</tcpNoDelay>
       <socketBufferSize>65536</socketBufferSize>
       
       <!-- Keep-alive settings -->
       <keepAlive>true</keepAlive>
       <keepAliveInterval>60</keepAliveInterval>
   </session>

SSL/TLS Configuration
~~~~~~~~~~~~~~~~~~~~~

For secure connections:

.. code-block:: xml

   <session>
       <useSSL>true</useSSL>
       <sslCertFile>/path/to/cert.pem</sslCertFile>
       <sslKeyFile>/path/to/key.pem</sslKeyFile>
       <sslCAFile>/path/to/ca.pem</sslCAFile>
       <sslVerifyPeer>true</sslVerifyPeer>
   </session>

Message Processing Configuration
--------------------------------

Message Validation
~~~~~~~~~~~~~~~~~~

Configure message validation settings:

.. code-block:: xml

   <session>
       <!-- Enable/disable message validation -->
       <validateMessages>true</validateMessages>
       
       <!-- Strict field validation -->
       <strictFieldValidation>true</strictFieldValidation>
       
       <!-- Maximum message size -->
       <maxMessageSize>8192</maxMessageSize>
       
       <!-- Message timeout settings -->
       <messageTimeout>30</messageTimeout>
   </session>

Sequence Number Management
~~~~~~~~~~~~~~~~~~~~~~~~~~

Configure sequence number handling:

.. code-block:: xml

   <session>
       <!-- Reset sequence numbers on startup -->
       <resetSeqNumFlag>false</resetSeqNumFlag>
       
       <!-- Starting sequence numbers -->
       <nextSenderMsgSeqNum>1</nextSenderMsgSeqNum>
       <nextTargetMsgSeqNum>1</nextTargetMsgSeqNum>
       
       <!-- Sequence number persistence -->
       <persistSequenceNumbers>true</persistSequenceNumbers>
       <sequenceNumberFile>data/seqnums.dat</sequenceNumberFile>
   </session>

Advanced Logging Configuration
------------------------------

Structured Logging
~~~~~~~~~~~~~~~~~~

Configure detailed logging for different components:

.. code-block:: xml

   <logging>
       <!-- Global log level -->
       <level>INFO</level>
       
       <!-- Component-specific logging -->
       <components>
           <fix>DEBUG</fix>
           <trading>INFO</trading>
           <network>WARN</network>
           <performance>INFO</performance>
       </components>
       
       <!-- Log formatting -->
       <timestampFormat>%Y-%m-%d %H:%M:%S.%f</timestampFormat>
       <includeThreadId>true</includeThreadId>
       <includeLocation>false</includeLocation>
   </logging>

Log Rotation and Archival
~~~~~~~~~~~~~~~~~~~~~~~~~

Configure log file management:

.. code-block:: xml

   <logging>
       <file>logs/fix-fasttrade.log</file>
       
       <!-- Rotation settings -->
       <rotateDaily>true</rotateDaily>
       <maxFileSize>100MB</maxFileSize>
       <maxFiles>30</maxFiles>
       
       <!-- Compression -->
       <compressRotatedFiles>true</compressRotatedFiles>
       
       <!-- Archive settings -->
       <archiveOldLogs>true</archiveOldLogs>
       <archiveDirectory>logs/archive</archiveDirectory>
   </logging>

Security Configuration
----------------------

Authentication Settings
~~~~~~~~~~~~~~~~~~~~~~~

Configure FIX session authentication:

.. code-block:: xml

   <session>
       <!-- Basic authentication -->
       <username>trader_001</username>
       <password>secure_password</password>
       
       <!-- Certificate-based authentication -->
       <useCertificateAuth>true</useCertificateAuth>
       <clientCertFile>/path/to/client.crt</clientCertFile>
       <clientKeyFile>/path/to/client.key</clientKeyFile>
   </session>

Access Control
~~~~~~~~~~~~~~

Configure access restrictions:

.. code-block:: xml

   <security>
       <!-- IP address restrictions -->
       <allowedIPs>
           <ip>192.168.1.0/24</ip>
           <ip>10.0.0.100</ip>
       </allowedIPs>
       
       <!-- Rate limiting -->
       <maxMessagesPerSecond>1000</maxMessagesPerSecond>
       <maxOrdersPerSecond>100</maxOrdersPerSecond>
       
       <!-- Session limits -->
       <maxConcurrentSessions>10</maxConcurrentSessions>
   </security>

Environment-Specific Configurations
-----------------------------------

Development Environment
~~~~~~~~~~~~~~~~~~~~~~~

Configuration optimized for development:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>DEV_TRADER</senderCompID>
           <targetCompID>DEV_EXCHANGE</targetCompID>
           <host>localhost</host>
           <port>9878</port>
           <heartBtInt>60</heartBtInt>
           <validateMessages>false</validateMessages>
           <resetSeqNumFlag>true</resetSeqNumFlag>
       </session>
       
       <logging>
           <level>DEBUG</level>
           <console>true</console>
           <file>logs/dev-fix-fasttrade.log</file>
           <components>
               <fix>DEBUG</fix>
               <trading>DEBUG</trading>
           </components>
       </logging>
       
       <performance>
           <cpuAffinity>false</cpuAffinity>
           <memoryLock>false</memoryLock>
           <threadPriority>0</threadPriority>
       </performance>
   </fix>

Staging Environment
~~~~~~~~~~~~~~~~~~~

Configuration for staging/testing:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <fix>
       <session>
           <senderCompID>STAGE_TRADER_001</senderCompID>
           <targetCompID>STAGE_EXCHANGE</targetCompID>
           <host>staging.exchange.com</host>
           <port>9878</port>
           <heartBtInt>30</heartBtInt>
           <validateMessages>true</validateMessages>
           <useSSL>true</useSSL>
       </session>
       
       <logging>
           <level>INFO</level>
           <file>logs/stage-fix-fasttrade.log</file>
           <rotateDaily>true</rotateDaily>
           <maxFiles>7</maxFiles>
       </logging>
       
       <performance>
           <cpuAffinity>true</cpuAffinity>
           <cpuMain>0</cpuMain>
           <cpuFix>1</cpuFix>
           <memoryLock>true</memoryLock>
           <threadPriority>-5</threadPriority>
       </performance>
   </fix>

Production Environment
~~~~~~~~~~~~~~~~~~~~~~

High-performance production configuration:

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
           <strictFieldValidation>true</strictFieldValidation>
           <useSSL>true</useSSL>
           <persistSequenceNumbers>true</persistSequenceNumbers>
       </session>
       
       <logging>
           <level>WARN</level>
           <file>/var/log/fix-fasttrade/fix-fasttrade.log</file>
           <rotateDaily>true</rotateDaily>
           <maxFiles>30</maxFiles>
           <compressRotatedFiles>true</compressRotatedFiles>
       </logging>
       
       <performance>
           <cpuAffinity>true</cpuAffinity>
           <cpuMain>0</cpuMain>
           <cpuFix>1</cpuFix>
           <cpuOrder>2</cpuOrder>
           <cpuMarket>3</cpuMarket>
           <memoryLock>true</memoryLock>
           <threadPriority>-15</threadPriority>
           <useRealTimeScheduling>true</useRealTimeScheduling>
       </performance>
       
       <security>
           <allowedIPs>
               <ip>10.0.1.0/24</ip>
           </allowedIPs>
           <maxMessagesPerSecond>10000</maxMessagesPerSecond>
           <maxOrdersPerSecond>1000</maxOrdersPerSecond>
       </security>
   </fix>

Configuration Management
------------------------

Configuration Validation
~~~~~~~~~~~~~~~~~~~~~~~~

Always validate configurations before deployment:

.. code-block:: bash

   # XML syntax validation
   xmllint --noout config/fix-config.xml
   
   # Application configuration validation
   ./bin/fix-fasttrade --config config/fix-config.xml --validate
   
   # Dry run test
   ./bin/fix-fasttrade --config config/fix-config.xml --dry-run

Configuration Templates
~~~~~~~~~~~~~~~~~~~~~~~

Use configuration templates for consistency:

.. code-block:: bash

   # Copy template
   cp config/templates/production-template.xml config/prod-config.xml
   
   # Customize for your environment
   sed -i 's/TEMPLATE_SENDER_ID/ACTUAL_SENDER_ID/g' config/prod-config.xml

Version Control
~~~~~~~~~~~~~~~

Keep configurations in version control:

.. code-block:: bash

   # Track configuration changes
   git add config/
   git commit -m "Update production configuration for new exchange"
   
   # Use environment-specific branches
   git checkout production
   git merge staging

Monitoring Configuration Changes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Monitor configuration file changes:

.. code-block:: bash

   # File integrity monitoring
   sha256sum config/fix-config.xml > config/fix-config.xml.sha256
   
   # Automated validation on changes
   inotifywait -m config/ -e modify --format '%w%f' | while read file; do
       xmllint --noout "$file" && echo "Configuration valid"
   done