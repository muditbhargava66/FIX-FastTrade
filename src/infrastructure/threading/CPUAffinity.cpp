/**
 * @file CPUAffinity.cpp
 * @brief Implementation of CPU affinity and thread pinning
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#include "infrastructure/threading/CPUAffinity.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <set>

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#elif defined(_WIN32)
#include <processthreadsapi.h>
#endif

namespace infrastructure {
namespace threading {

// Static member initialization
std::optional<std::vector<CPUCore>> CPUAffinity::cachedTopology_;
std::optional<std::vector<NUMANode>> CPUAffinity::cachedNUMATopology_;

bool CPUAffinity::setCurrentThreadAffinity(int coreId) {
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreId, &cpuset);
    
    pthread_t thread = pthread_self();
    return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) == 0;
    
#elif defined(_WIN32)
    DWORD_PTR mask = 1ULL << coreId;
    return SetThreadAffinityMask(GetCurrentThread(), mask) != 0;
    
#elif defined(__APPLE__)
    // macOS doesn't support thread affinity directly
    // We can use thread affinity policy as a hint
    thread_affinity_policy_data_t policy = { static_cast<integer_t>(coreId) };
    thread_port_t thread = pthread_mach_thread_np(pthread_self());
    
    return thread_policy_set(thread, THREAD_AFFINITY_POLICY,
                            (thread_policy_t)&policy, 1) == KERN_SUCCESS;
#else
    return false;
#endif
}

bool CPUAffinity::setThreadAffinity(std::thread& thread, int coreId) {
    (void)thread;  // Suppress unused parameter warning
    (void)coreId;  // Suppress unused parameter warning
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreId, &cpuset);
    
    return pthread_setaffinity_np(thread.native_handle(), 
                                 sizeof(cpu_set_t), &cpuset) == 0;
    
#elif defined(_WIN32)
    DWORD_PTR mask = 1ULL << coreId;
    return SetThreadAffinityMask(thread.native_handle(), mask) != 0;
    
#else
    return false;
#endif
}

bool CPUAffinity::setCurrentThreadAffinity(const std::vector<int>& cores) {
    if (cores.empty()) {
        return false;
    }
    
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    for (int core : cores) {
        CPU_SET(core, &cpuset);
    }
    
    pthread_t thread = pthread_self();
    return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) == 0;
    
#elif defined(_WIN32)
    DWORD_PTR mask = 0;
    for (int core : cores) {
        mask |= (1ULL << core);
    }
    return SetThreadAffinityMask(GetCurrentThread(), mask) != 0;
    
#else
    // Fall back to setting affinity to first core in list
    return setCurrentThreadAffinity(cores[0]);
#endif
}

std::vector<int> CPUAffinity::getCurrentThreadAffinity() {
    std::vector<int> cores;
    
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    pthread_t thread = pthread_self();
    if (pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset) == 0) {
        for (int i = 0; i < CPU_SETSIZE; ++i) {
            if (CPU_ISSET(i, &cpuset)) {
                cores.push_back(i);
            }
        }
    }
    
#elif defined(_WIN32)
    DWORD_PTR processMask, systemMask;
    if (GetProcessAffinityMask(GetCurrentProcess(), &processMask, &systemMask)) {
        for (int i = 0; i < 64; ++i) {
            if (processMask & (1ULL << i)) {
                cores.push_back(i);
            }
        }
    }
#endif
    
    return cores;
}

bool CPUAffinity::setProcessPriority(Priority priority) {
    (void)priority;  // Suppress unused parameter warning
#ifdef __linux__
    int niceness = static_cast<int>(priority);
    return setpriority(PRIO_PROCESS, 0, niceness) == 0;
    
#elif defined(_WIN32)
    HANDLE process = GetCurrentProcess();
    DWORD priorityClass = NORMAL_PRIORITY_CLASS;
    
    switch (priority) {
        case Priority::IDLE:
            priorityClass = IDLE_PRIORITY_CLASS;
            break;
        case Priority::LOWEST:
        case Priority::BELOW_NORMAL:
            priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
            break;
        case Priority::ABOVE_NORMAL:
            priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
            break;
        case Priority::HIGHEST:
            priorityClass = HIGH_PRIORITY_CLASS;
            break;
        case Priority::REALTIME:
            priorityClass = REALTIME_PRIORITY_CLASS;
            break;
        default:
            priorityClass = NORMAL_PRIORITY_CLASS;
    }
    
    return SetPriorityClass(process, priorityClass) != 0;
    
#else
    return false;
#endif
}

bool CPUAffinity::setCurrentThreadPriority(Priority priority) {
    (void)priority;  // Suppress unused parameter warning
#ifdef __linux__
    struct sched_param param;
    param.sched_priority = static_cast<int>(priority);
    
    pthread_t thread = pthread_self();
    return pthread_setschedparam(thread, SCHED_OTHER, &param) == 0;
    
#elif defined(_WIN32)
    int threadPriority = THREAD_PRIORITY_NORMAL;
    
    switch (priority) {
        case Priority::IDLE:
            threadPriority = THREAD_PRIORITY_IDLE;
            break;
        case Priority::LOWEST:
            threadPriority = THREAD_PRIORITY_LOWEST;
            break;
        case Priority::BELOW_NORMAL:
            threadPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case Priority::ABOVE_NORMAL:
            threadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case Priority::HIGHEST:
            threadPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case Priority::REALTIME:
            threadPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
        default:
            threadPriority = THREAD_PRIORITY_NORMAL;
    }
    
    return SetThreadPriority(GetCurrentThread(), threadPriority) != 0;
    
#else
    return false;
#endif
}

unsigned int CPUAffinity::getNumCores() {
    return std::thread::hardware_concurrency();
}

unsigned int CPUAffinity::getNumPhysicalCores() {
#ifdef __linux__
    // Parse /proc/cpuinfo to get physical core count
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    std::set<std::pair<int, int>> physical_cores;
    
    int physical_id = -1;
    int core_id = -1;
    
    while (std::getline(cpuinfo, line)) {
        if (line.find("physical id") != std::string::npos) {
            sscanf(line.c_str(), "physical id : %d", &physical_id);
        } else if (line.find("core id") != std::string::npos) {
            sscanf(line.c_str(), "core id : %d", &core_id);
            if (physical_id != -1 && core_id != -1) {
                physical_cores.insert({physical_id, core_id});
            }
        }
    }
    
    return physical_cores.empty() ? getNumCores() / 2 : physical_cores.size();
    
#elif defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    
    // This is approximate - assumes hyperthreading if logical > physical
    DWORD logical = sysinfo.dwNumberOfProcessors;
    return logical / 2;  // Rough estimate
    
#else
    // Fallback: assume half of logical cores are physical
    return getNumCores() / 2;
#endif
}

std::vector<CPUCore> CPUAffinity::getCPUTopology() {
    std::vector<CPUCore> topology;
    
    // Simple implementation - just create basic topology
    unsigned int numCores = getNumCores();
    for (unsigned int i = 0; i < numCores; ++i) {
        CPUCore core;
        core.id = static_cast<int>(i);
        core.physicalPackage = 0;  // Assume single package
        core.logicalProcessor = static_cast<int>(i);
        core.isHyperthread = (i >= getNumPhysicalCores());
        topology.push_back(core);
    }
    
    return topology;
}

int CPUAffinity::getCurrentCPU() {
#ifdef __linux__
    return sched_getcpu();
    
#elif defined(_WIN32)
    return GetCurrentProcessorNumber();
    
#else
    return -1;
#endif
}

void CPUAffinity::yield() {
    std::this_thread::yield();
}

void CPUAffinity::cpuRelax() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    // x86/x64: PAUSE instruction
    __builtin_ia32_pause();
#elif defined(__aarch64__)
    // ARM64: YIELD instruction
    __asm__ __volatile__("yield" ::: "memory");
#else
    // Fallback to compiler barrier
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

CPUAffinity::OptimalPlacement CPUAffinity::calculateOptimalPlacement(
    int numCriticalThreads,
    int numNormalThreads,
    int numBackgroundThreads) {
    
    OptimalPlacement placement;
    placement.avoidHyperthreads = true;
    placement.respectNUMA = true;
    
    auto topology = getCPUTopology();
    
    // Separate physical cores from hyperthreads
    std::vector<int> physicalCores;
    std::vector<int> hyperthreadCores;
    
    for (const auto& core : topology) {
        if (!core.isHyperthread) {
            physicalCores.push_back(core.id);
        } else {
            hyperthreadCores.push_back(core.id);
        }
    }
    
    // Assign critical threads to physical cores first
    int assigned = 0;
    for (int i = 0; i < numCriticalThreads && static_cast<size_t>(i) < physicalCores.size(); ++i) {
        placement.criticalThreadCores.push_back(physicalCores[i]);
        assigned++;
    }
    
    // Assign normal threads to remaining physical cores
    for (int i = 0; i < numNormalThreads && assigned < static_cast<int>(physicalCores.size()); ++i) {
        placement.normalThreadCores.push_back(physicalCores[assigned]);
        assigned++;
    }
    
    // Assign background threads to hyperthreads or remaining cores
    for (int i = 0; i < numBackgroundThreads; ++i) {
        if (static_cast<size_t>(i) < hyperthreadCores.size()) {
            placement.backgroundThreadCores.push_back(hyperthreadCores[i]);
        } else if (assigned < static_cast<int>(physicalCores.size())) {
            placement.backgroundThreadCores.push_back(physicalCores[assigned]);
            assigned++;
        }
    }
    
    return placement;
}

// ScopedThreadAffinity implementation
ScopedThreadAffinity::ScopedThreadAffinity(int coreId) {
    originalAffinity_ = CPUAffinity::getCurrentThreadAffinity();
    CPUAffinity::setCurrentThreadAffinity(coreId);
}

ScopedThreadAffinity::ScopedThreadAffinity(const std::vector<int>& cores) {
    originalAffinity_ = CPUAffinity::getCurrentThreadAffinity();
    CPUAffinity::setCurrentThreadAffinity(cores);
}

ScopedThreadAffinity::~ScopedThreadAffinity() {
    restore();
}

void ScopedThreadAffinity::restore() {
    if (!restored_ && !originalAffinity_.empty()) {
        CPUAffinity::setCurrentThreadAffinity(originalAffinity_);
        restored_ = true;
    }
}

ScopedThreadAffinity::ScopedThreadAffinity(ScopedThreadAffinity&& other) noexcept
    : originalAffinity_(std::move(other.originalAffinity_)),
      restored_(other.restored_) {
    other.restored_ = true;  // Prevent other from restoring
}

ScopedThreadAffinity& ScopedThreadAffinity::operator=(ScopedThreadAffinity&& other) noexcept {
    if (this != &other) {
        restore();
        originalAffinity_ = std::move(other.originalAffinity_);
        restored_ = other.restored_;
        other.restored_ = true;
    }
    return *this;
}

} // namespace threading
} // namespace infrastructure
