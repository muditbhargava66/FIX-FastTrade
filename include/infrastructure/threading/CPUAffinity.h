/**
 * @file CPUAffinity.h
 * @brief CPU affinity and thread pinning for low-latency performance
 * 
 * Provides platform-specific implementations for binding threads to
 * specific CPU cores, essential for reducing context switches and
 * improving cache locality in high-frequency trading systems.
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef INFRASTRUCTURE_THREADING_CPU_AFFINITY_H
#define INFRASTRUCTURE_THREADING_CPU_AFFINITY_H

#include <thread>
#include <vector>
#include <string>
#include <optional>
#include <bitset>

#ifdef __linux__
#include <sched.h>
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

namespace infrastructure {
namespace threading {

/**
 * @brief CPU core information
 */
struct CPUCore {
    int id;                     // Core ID
    int physicalPackage;        // Physical CPU package/socket
    int logicalProcessor;       // Logical processor number
    bool isHyperthread;         // True if this is a hyperthread sibling
    std::vector<int> cacheSharing; // Other cores sharing cache
};

/**
 * @brief NUMA node information
 */
struct NUMANode {
    int id;                     // NUMA node ID
    std::vector<int> cores;     // CPU cores in this node
    size_t memorySize;          // Memory size in bytes
};

/**
 * @brief CPU affinity and topology management
 * 
 * Features:
 * - Thread-to-core binding
 * - NUMA awareness
 * - Hyperthread detection
 * - Process priority management
 * - CPU isolation for dedicated cores
 */
class CPUAffinity {
public:
    /**
     * @brief Thread priority levels
     */
    enum class Priority {
        IDLE = -20,
        LOWEST = -10,
        BELOW_NORMAL = -5,
        NORMAL = 0,
        ABOVE_NORMAL = 5,
        HIGHEST = 10,
        REALTIME = 20
    };
    
    /**
     * @brief Scheduling policy
     */
    enum class SchedulingPolicy {
        DEFAULT,    // Default OS scheduling
        FIFO,       // Real-time FIFO
        RR,         // Real-time round-robin
        BATCH,      // Batch processing
        IDLE        // Only run when idle
    };
    
    /**
     * @brief Binds current thread to a specific CPU core
     * @param coreId CPU core ID (0-based)
     * @return true if successful
     */
    static bool setCurrentThreadAffinity(int coreId);
    
    /**
     * @brief Binds a thread to a specific CPU core
     * @param thread Thread to bind
     * @param coreId CPU core ID (0-based)
     * @return true if successful
     */
    static bool setThreadAffinity(std::thread& thread, int coreId);
    
    /**
     * @brief Binds current thread to multiple CPU cores
     * @param cores Vector of core IDs
     * @return true if successful
     */
    static bool setCurrentThreadAffinity(const std::vector<int>& cores);
    
    /**
     * @brief Gets current thread's CPU affinity
     * @return Vector of core IDs thread can run on
     */
    static std::vector<int> getCurrentThreadAffinity();
    
    /**
     * @brief Sets process priority
     * @param priority Priority level
     * @return true if successful
     */
    static bool setProcessPriority(Priority priority);
    
    /**
     * @brief Sets thread priority
     * @param thread Thread to set priority for
     * @param priority Priority level
     * @return true if successful
     */
    static bool setThreadPriority(std::thread& thread, Priority priority);
    
    /**
     * @brief Sets current thread priority
     * @param priority Priority level
     * @return true if successful
     */
    static bool setCurrentThreadPriority(Priority priority);
    
    /**
     * @brief Sets thread scheduling policy
     * @param thread Thread to set policy for
     * @param policy Scheduling policy
     * @return true if successful
     */
    static bool setThreadSchedulingPolicy(std::thread& thread, SchedulingPolicy policy);
    
    /**
     * @brief Gets number of available CPU cores
     * @return Total number of logical CPU cores
     */
    static unsigned int getNumCores();
    
    /**
     * @brief Gets number of physical CPU cores (excluding hyperthreads)
     * @return Number of physical cores
     */
    static unsigned int getNumPhysicalCores();
    
    /**
     * @brief Gets detailed CPU topology
     * @return Vector of CPU core information
     */
    static std::vector<CPUCore> getCPUTopology();
    
    /**
     * @brief Gets NUMA topology
     * @return Vector of NUMA node information
     */
    static std::vector<NUMANode> getNUMATopology();
    
    /**
     * @brief Checks if a CPU core is isolated
     * @param coreId CPU core ID
     * @return true if core is isolated from general scheduling
     */
    static bool isCoreIsolated(int coreId);
    
    /**
     * @brief Isolates a CPU core from general scheduling
     * @param coreId CPU core ID to isolate
     * @return true if successful (requires appropriate privileges)
     * 
     * Note: This typically requires root/admin privileges and
     * may need kernel boot parameters on some systems
     */
    static bool isolateCPUCore(int coreId);
    
    /**
     * @brief Gets current CPU core the calling thread is running on
     * @return CPU core ID, or -1 if unable to determine
     */
    static int getCurrentCPU();
    
    /**
     * @brief Yields current thread's time slice
     * 
     * Useful for spinlocks and busy-wait loops
     */
    static void yield();
    
    /**
     * @brief Provides a CPU relaxation hint
     * 
     * Uses platform-specific instructions (PAUSE on x86)
     * to improve performance of spin-wait loops
     */
    static void cpuRelax();
    
    /**
     * @brief Configuration for optimal thread placement
     */
    struct OptimalPlacement {
        std::vector<int> criticalThreadCores;    // Cores for critical threads
        std::vector<int> normalThreadCores;      // Cores for normal threads
        std::vector<int> backgroundThreadCores;  // Cores for background tasks
        bool avoidHyperthreads;                  // Whether to avoid HT siblings
        bool respectNUMA;                        // Whether to respect NUMA boundaries
    };
    
    /**
     * @brief Calculates optimal thread placement based on system topology
     * @param numCriticalThreads Number of critical low-latency threads
     * @param numNormalThreads Number of normal priority threads
     * @param numBackgroundThreads Number of background threads
     * @return Optimal placement configuration
     */
    static OptimalPlacement calculateOptimalPlacement(
        int numCriticalThreads,
        int numNormalThreads,
        int numBackgroundThreads
    );
    
    /**
     * @brief Applies optimal placement to named threads
     * @param placement Optimal placement configuration
     * @param criticalThreadNames Names of critical threads
     * @param normalThreadNames Names of normal threads
     * @param backgroundThreadNames Names of background threads
     * @return true if all placements successful
     */
    static bool applyOptimalPlacement(
        const OptimalPlacement& placement,
        const std::vector<std::string>& criticalThreadNames,
        const std::vector<std::string>& normalThreadNames,
        const std::vector<std::string>& backgroundThreadNames
    );
    
private:
    // Platform-specific helper functions
    #ifdef __linux__
    static cpu_set_t vectorToCpuSet(const std::vector<int>& cores);
    static std::vector<int> cpuSetToVector(const cpu_set_t& cpuset);
    #endif
    
    // Cache for topology information
    static std::optional<std::vector<CPUCore>> cachedTopology_;
    static std::optional<std::vector<NUMANode>> cachedNUMATopology_;
};

/**
 * @brief RAII wrapper for thread affinity
 * 
 * Automatically restores original affinity on destruction
 */
class ScopedThreadAffinity {
private:
    std::vector<int> originalAffinity_;
    bool restored_ = false;
    
public:
    /**
     * @brief Sets thread affinity and saves original
     * @param coreId CPU core to bind to
     */
    explicit ScopedThreadAffinity(int coreId);
    
    /**
     * @brief Sets thread affinity and saves original
     * @param cores CPU cores to bind to
     */
    explicit ScopedThreadAffinity(const std::vector<int>& cores);
    
    /**
     * @brief Restores original affinity
     */
    ~ScopedThreadAffinity();
    
    // Delete copy operations
    ScopedThreadAffinity(const ScopedThreadAffinity&) = delete;
    ScopedThreadAffinity& operator=(const ScopedThreadAffinity&) = delete;
    
    // Allow move operations
    ScopedThreadAffinity(ScopedThreadAffinity&& other) noexcept;
    ScopedThreadAffinity& operator=(ScopedThreadAffinity&& other) noexcept;
    
    /**
     * @brief Manually restore original affinity
     */
    void restore();
};

} // namespace threading
} // namespace infrastructure

#endif // INFRASTRUCTURE_THREADING_CPU_AFFINITY_H
