# FIX API

This document provides an overview of the FIX API provided by the FIX-FastTrade system. The FIX API allows external trading systems to integrate with FIX-FastTrade using the Financial Information eXchange (FIX) protocol.

SessionID Class
---------------

The ``SessionID`` class represents a unique identifier for FIX sessions.

Constructors
~~~~~~~~~~~~

.. code-block:: cpp

// Default constructor
SessionID();

// Constructor from string (for backward compatibility)
SessionID(const std::string& sessionId);

// Constructor with FIX version, sender, and target
SessionID(const std::string& beginString, const std::string& senderCompID, const std::string& targetCompID);

// Copy constructor
SessionID(const SessionID& other);


Methods
~~~~~~~

.. code-block:: cpp

// String conversion
std::string toString() const;
operator std::string() const;

// Getters
const std::string& getBeginString() const;
const std::string& getSenderCompID() const;
const std::string& getTargetCompID() const;

// Comparison operators
bool operator==(const SessionID& other) const;
bool operator!=(const SessionID& other) const;
bool operator<(const SessionID& other) const;


Session Management
------------------

The enhanced ``Session`` class provides high-performance FIX session management with connection pooling and optimizations.

Session Configuration
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

struct SessionConfig {
    // Connection settings
    std::string host;
    uint16_t port;
    std::vector<std::pair<std::string, uint16_t>> backupHosts;
    
    // Performance settings
    size_t sendQueueSize = 10000;
    size_t receiveQueueSize = 10000;
    bool useZeroCopy = true;
    bool enableConnectionPool = true;
    size_t connectionPoolSize = 5;
    
    // Timing settings
    std::chrono::milliseconds heartbeatInterval{30000};
    std::chrono::milliseconds reconnectInterval{5000};
    std::chrono::milliseconds responseTimeout{5000};
    
    // Threading settings
    int sendThreadCPU = -1;  // CPU core for send thread (-1 = no affinity)
    int receiveThreadCPU = -1;  // CPU core for receive thread
    
    // Monitoring settings
    bool enableLatencyTracking = true;
    size_t latencyHistorySize = 10000;
};


Session Initialization
~~~~~~~~~~~~~~~~~~~~~~

To create a FIX session, use the Session constructor:

.. code-block:: cpp

Session(const Config& config, 
        const SessionConfig& sessionConfig,
        Application& application);


* ``config``: The FIX configuration object
* ``sessionConfig``: Enhanced session configuration with performance settings
* ``application``: The application object implementing the ``Application`` interface

Connection Management
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Connect to the FIX endpoint
bool connect();

// Check if session is connected
bool isConnected() const;

// Check if session is logged on
bool isLoggedOn() const;

// Disconnect from the FIX endpoint
void disconnect();


Message Handling
----------------

The FIX API provides high-performance functions to send and receive FIX messages with zero-copy optimization.

Sending Messages
~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Send a message (zero-copy if possible)
bool send(const Message& message);

// Send a pre-formatted message buffer
bool sendRaw(const char* buffer, size_t length);


Receiving Messages
~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Receive a message (blocking)
bool receive(Message& message, 
             std::optional<std::chrono::milliseconds> timeout = std::nullopt);

// Try to receive a message (non-blocking)
bool tryReceive(Message& message);


Application Interface
~~~~~~~~~~~~~~~~~~~~~

Implement the ``Application`` interface to handle session events and messages:

.. code-block:: cpp

class Application {
public:
    virtual ~Application() = default;

    // Session lifecycle callbacks
    virtual void onCreate(const SessionID& sessionID) = 0;
    virtual void onLogon(const SessionID& sessionID) = 0;
    virtual void onLogout(const SessionID& sessionID) = 0;

    // Administrative message callbacks
    virtual void toAdmin(Message& message, const SessionID& sessionID) = 0;
    virtual void fromAdmin(const Message& message, const SessionID& sessionID) = 0;

    // Application message callbacks
    virtual void toApp(Message& message, const SessionID& sessionID) = 0;
    virtual void fromApp(const Message& message, const SessionID& sessionID) = 0;
};


Message Class
-------------

The ``Message`` class represents FIX messages with field-based access.

Methods
~~~~~~~

.. code-block:: cpp

// Field manipulation
void setField(int tag, const std::string& value);
std::string getField(int tag) const;
bool hasField(int tag) const;
void removeField(int tag);
void clear();

// Access all fields
const std::unordered_map<int, std::string>& getFields() const;


Session State Management
------------------------

Session States
~~~~~~~~~~~~~~

.. code-block:: cpp

enum class SessionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    LOGGING_ON,
    LOGGED_ON,
    LOGGING_OUT,
    ERROR
};


State Queries
~~~~~~~~~~~~~

.. code-block:: cpp

// Get current session state
SessionState getState() const;

// Get session ID
const SessionID& getSessionID() const;


Performance Features
--------------------

Connection Pooling
~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Enable/disable connection pooling
void setConnectionPoolEnabled(bool enabled);

// Set automatic failover hosts
void setFailoverHosts(const std::vector<std::pair<std::string, uint16_t>>& hosts);


Statistics and Monitoring
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

// Get session statistics
std::shared_ptr<SessionStatistics> getStatistics() const;


The ``SessionStatistics`` class provides comprehensive metrics including:
* Message counts and rates
* Latency percentiles (min, max, mean, p50, p95, p99, p999)
* Connection statistics
* Error counts

Advanced Features
-----------------

Zero-Copy Message Handling
~~~~~~~~~~~~~~~~~~~~~~~~~~

The session supports zero-copy message processing for optimal performance when ``useZeroCopy`` is enabled in the session configuration.

Thread Affinity
~~~~~~~~~~~~~~~

Configure CPU affinity for send and receive threads to optimize performance:

.. code-block:: cpp

SessionConfig config;
config.sendThreadCPU = 2;      // Pin send thread to CPU core 2
config.receiveThreadCPU = 3;   // Pin receive thread to CPU core 3


Automatic Failover
~~~~~~~~~~~~~~~~~~

Configure backup hosts for automatic failover:

.. code-block:: cpp

SessionConfig config;
config.backupHosts = {
    {"backup1.example.com", 9001},
    {"backup2.example.com", 9001}
};


For more detailed information on the FIX API and its usage, please refer to the FIX protocol specification and the FIX-FastTrade documentation.

