/**
 * @file Session.h
 * @brief Enhanced FIX session management with connection pooling and optimizations
 * 
 * This enhanced version includes:
 * - Connection pooling for reduced connection overhead
 * - Lock-free message queues for better throughput
 * - Zero-copy message handling
 * - Automatic failover support
 * - Latency tracking and monitoring
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef FIX_SESSION_H
#define FIX_SESSION_H

#include "fix/Config.h"
#include "fix/Application.h"
#include "fix/Message.h"
#include "fix/ZeroCopyMessage.h"
#include "infrastructure/containers/SimpleQueue.h"
#include "infrastructure/memory/SimplePool.h"
#include "infrastructure/threading/CPUAffinity.h"
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>

namespace fix {

// Forward declarations
class ConnectionPool;
class SessionStatistics;

/**
 * @brief Session state enumeration
 */
enum class SessionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    LOGGING_ON,
    LOGGED_ON,
    LOGGING_OUT,
    ERROR
};

/**
 * @brief Enhanced FIX session with high-performance features
 * 
 * Features:
 * - Connection pooling for reduced latency
 * - Lock-free message queues
 * - Zero-copy message parsing
 * - Automatic reconnection and failover
 * - Comprehensive latency tracking
 * - Thread-safe operations
 */
class Session {
public:
    /**
     * @brief Session configuration
     */
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
    
    /**
     * @brief Constructs a session with enhanced configuration
     * @param config FIX configuration
     * @param sessionConfig Enhanced session configuration
     * @param application Application callback interface
     */
    Session(const Config& config, 
            const SessionConfig& sessionConfig,
            Application& application);
    
    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~Session();
    
    // Delete copy operations
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    
    /**
     * @brief Connects to the FIX endpoint
     * @return true if connection successful
     */
    bool connect();
    
    /**
     * @brief Checks if session is connected
     * @return true if connected
     */
    bool isConnected() const;
    
    /**
     * @brief Disconnects from the FIX endpoint
     */
    void disconnect();
    
    /**
     * @brief Sends a message (zero-copy if possible)
     * @param message Message to send
     * @return true if message queued successfully
     */
    bool send(const Message& message);
    
    /**
     * @brief Sends a pre-formatted message buffer
     * @param buffer Message buffer
     * @param length Buffer length
     * @return true if message queued successfully
     */
    bool sendRaw(const char* buffer, size_t length);
    
    /**
     * @brief Receives a message (blocking)
     * @param message Output message
     * @param timeout Optional timeout
     * @return true if message received
     */
    bool receive(Message& message, 
                 std::optional<std::chrono::milliseconds> timeout = std::nullopt);
    
    /**
     * @brief Tries to receive a message (non-blocking)
     * @param message Output message
     * @return true if message available
     */
    bool tryReceive(Message& message);
    
    /**
     * @brief Gets current session state
     */
    SessionState getState() const { 
        return state_.load(std::memory_order_acquire); 
    }
    
    /**
     * @brief Checks if session is logged on
     */
    bool isLoggedOn() const { 
        return getState() == SessionState::LOGGED_ON; 
    }
    
    /**
     * @brief Gets session ID
     */
    const SessionID& getSessionID() const { return sessionID_; }
    
    /**
     * @brief Gets session statistics
     */
    std::shared_ptr<SessionStatistics> getStatistics() const { 
        return statistics_; 
    }
    
    /**
     * @brief Enables/disables connection pooling
     */
    void setConnectionPoolEnabled(bool enabled);
    
    /**
     * @brief Sets automatic failover hosts
     */
    void setFailoverHosts(const std::vector<std::pair<std::string, uint16_t>>& hosts);
    
private:
    // Configuration
    SessionConfig sessionConfig_;
    Application& application_;
    
    // Session state
    std::atomic<SessionState> state_{SessionState::DISCONNECTED};
    SessionID sessionID_;
    
    // Networking
    std::unique_ptr<boost::asio::io_context> ioContext_;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_;
    std::unique_ptr<ConnectionPool> connectionPool_;
    
    // Message queues (thread-safe)
    infrastructure::containers::SimpleQueue<std::unique_ptr<Message>> sendQueue_;
    infrastructure::containers::SimpleQueue<std::unique_ptr<Message>> receiveQueue_;
    
    // Memory pools for efficient allocation
    infrastructure::memory::SimplePool<Message> messagePool_;
    infrastructure::memory::SimplePool<char> bufferPool_;
    
    // Threading
    std::thread sendThread_;
    std::thread receiveThread_;
    std::atomic<bool> running_{false};
    
    // Statistics
    std::shared_ptr<SessionStatistics> statistics_;
    
    // Sequence numbers
    std::atomic<uint32_t> outgoingSeqNum_{1};
    std::atomic<uint32_t> incomingSeqNum_{1};
    
    // Heartbeat management
    std::chrono::steady_clock::time_point lastSentTime_;
    std::chrono::steady_clock::time_point lastReceivedTime_;
    
    // Private methods
    void sendThreadFunc();
    void receiveThreadFunc();
    void handleConnectionLoss();
    void attemptReconnect();
    void processIncomingMessage(const ZeroCopyMessage& msg);
    void sendHeartbeatIfNeeded();
    void updateStatistics(const Message& msg, bool sent);
    
    /**
     * @brief Logs a message with latency tracking
     */
    void logMessage(const Message& message, bool sent, 
                   std::chrono::nanoseconds latency = std::chrono::nanoseconds{0});
};

/**
 * @brief Connection pool for session connections
 * 
 * Maintains a pool of pre-established connections to reduce
 * connection setup latency during failover or reconnection.
 */
class ConnectionPool {
public:
    struct Connection {
        std::unique_ptr<boost::asio::ip::tcp::socket> socket;
        std::string host;
        uint16_t port;
        std::chrono::steady_clock::time_point lastUsed;
        bool inUse = false;
    };
    
    /**
     * @brief Constructs a connection pool
     * @param ioContext ASIO IO context
     * @param size Maximum pool size
     */
    ConnectionPool(boost::asio::io_context& ioContext, size_t size);
    
    /**
     * @brief Acquires a connection from the pool
     * @param host Target host
     * @param port Target port
     * @return Connection or nullptr if none available
     */
    std::unique_ptr<boost::asio::ip::tcp::socket> acquire(const std::string& host, 
                                                         uint16_t port);
    
    /**
     * @brief Returns a connection to the pool
     * @param socket Socket to return
     * @param host Connection host
     * @param port Connection port
     */
    void release(std::unique_ptr<boost::asio::ip::tcp::socket> socket,
                const std::string& host, 
                uint16_t port);
    
    /**
     * @brief Pre-establishes connections to hosts
     * @param hosts List of host:port pairs
     */
    void preConnect(const std::vector<std::pair<std::string, uint16_t>>& hosts);
    
    /**
     * @brief Cleans up idle connections
     * @param maxIdleTime Maximum idle time before cleanup
     */
    void cleanupIdle(std::chrono::seconds maxIdleTime);
    
private:
    boost::asio::io_context& ioContext_;
    std::vector<Connection> connections_;
    std::mutex mutex_;
    size_t maxSize_;
};

/**
 * @brief Session statistics and monitoring
 * 
 * Tracks comprehensive session metrics including:
 * - Message counts and rates
 * - Latency percentiles
 * - Connection statistics
 * - Error counts
 */
class SessionStatistics {
public:
    struct LatencyStats {
        std::chrono::nanoseconds min{std::chrono::nanoseconds::max()};
        std::chrono::nanoseconds max{0};
        std::chrono::nanoseconds mean{0};
        std::chrono::nanoseconds p50{0};
        std::chrono::nanoseconds p95{0};
        std::chrono::nanoseconds p99{0};
        std::chrono::nanoseconds p999{0};
    };
    
    struct MessageStats {
        uint64_t sent = 0;
        uint64_t received = 0;
        uint64_t errors = 0;
        uint64_t rejects = 0;
    };
    
    struct ConnectionStats {
        uint32_t connects = 0;
        uint32_t disconnects = 0;
        uint32_t failovers = 0;
        std::chrono::steady_clock::time_point connectedSince;
    };
    
    /**
     * @brief Records a message latency
     * @param latency Message latency
     * @param sent true if sent, false if received
     */
    void recordLatency(std::chrono::nanoseconds latency, bool sent);
    
    /**
     * @brief Increments message counter
     * @param sent true if sent, false if received
     */
    void incrementMessageCount(bool sent);
    
    /**
     * @brief Records an error
     */
    void recordError();
    
    /**
     * @brief Records a reject
     */
    void recordReject();
    
    /**
     * @brief Records a connection event
     */
    void recordConnection();
    
    /**
     * @brief Records a disconnection event
     */
    void recordDisconnection();
    
    /**
     * @brief Records a failover event
     */
    void recordFailover();
    
    /**
     * @brief Gets send latency statistics
     */
    LatencyStats getSendLatencyStats() const;
    
    /**
     * @brief Gets receive latency statistics
     */
    LatencyStats getReceiveLatencyStats() const;
    
    /**
     * @brief Gets message statistics
     */
    MessageStats getMessageStats() const { 
        MessageStats stats;
        stats.sent = messageStats_.sent.load();
        stats.received = messageStats_.received.load();
        stats.errors = messageStats_.errors.load();
        stats.rejects = messageStats_.rejects.load();
        return stats;
    }
    
    /**
     * @brief Gets connection statistics
     */
    ConnectionStats getConnectionStats() const { 
        ConnectionStats stats;
        stats.connects = connectionStats_.connects.load();
        stats.disconnects = connectionStats_.disconnects.load();
        stats.failovers = connectionStats_.failovers.load();
        stats.connectedSince = connectionStats_.connectedSince;
        return stats;
    }
    
    /**
     * @brief Resets all statistics
     */
    void reset();
    
private:
    struct AtomicMessageStats {
        std::atomic<uint64_t> sent{0};
        std::atomic<uint64_t> received{0};
        std::atomic<uint64_t> errors{0};
        std::atomic<uint64_t> rejects{0};
    };
    
    struct AtomicConnectionStats {
        std::atomic<uint32_t> connects{0};
        std::atomic<uint32_t> disconnects{0};
        std::atomic<uint32_t> failovers{0};
        std::chrono::steady_clock::time_point connectedSince;
    };
    
    AtomicMessageStats messageStats_;
    AtomicConnectionStats connectionStats_;
    
    // Circular buffers for latency tracking
    mutable std::mutex latencyMutex_;
    boost::circular_buffer<std::chrono::nanoseconds> sendLatencies_{10000};
    boost::circular_buffer<std::chrono::nanoseconds> receiveLatencies_{10000};
    
    // Helper to calculate percentiles
    template<typename Container>
    LatencyStats calculateStats(const Container& latencies) const;
};

}  // namespace fix

#endif  // FIX_SESSION_H
