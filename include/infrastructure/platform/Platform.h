/**
 * @file Platform.h
 * @brief Platform abstraction layer for cross-platform compatibility
 * 
 * This header provides platform-specific definitions and abstractions
 * to ensure the codebase works across Windows, macOS, and Linux.
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef INFRASTRUCTURE_PLATFORM_H
#define INFRASTRUCTURE_PLATFORM_H

#include <cstdint>
#include <chrono>
#include <thread>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #define PLATFORM_POSIX 0
#elif defined(__APPLE__)
    #define PLATFORM_MACOS 1
    #define PLATFORM_POSIX 1
    #define PLATFORM_WINDOWS 0
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
    #define PLATFORM_POSIX 1
    #define PLATFORM_WINDOWS 0
#else
    #define PLATFORM_UNKNOWN 1
    #define PLATFORM_POSIX 0
    #define PLATFORM_WINDOWS 0
#endif

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH_X86_64 1
    #define ARCH_ARM64 0
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH_ARM64 1
    #define ARCH_X86_64 0
#elif defined(__i386) || defined(_M_IX86)
    #define ARCH_X86_32 1
    #define ARCH_X86_64 0
    #define ARCH_ARM64 0
#else
    #define ARCH_UNKNOWN 1
    #define ARCH_X86_64 0
    #define ARCH_ARM64 0
#endif

// Compiler detection
#if defined(__GNUC__)
    #define COMPILER_GCC 1
#elif defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#endif

// SIMD support detection
#if ARCH_X86_64 || defined(ARCH_X86_32)
    #define HAVE_SSE2 1
    #if defined(__AVX2__)
        #define HAVE_AVX2 1
    #endif
    #if defined(__AVX512F__)
        #define HAVE_AVX512 1
    #endif
#elif ARCH_ARM64
    #define HAVE_NEON 1
#endif

// Platform-specific includes
#if PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#elif PLATFORM_POSIX
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
#endif

namespace infrastructure {
namespace platform {

/**
 * @brief Platform information structure
 */
struct PlatformInfo {
    const char* name;
    const char* architecture;
    const char* compiler;
    bool hasSIMD;
    bool hasThreads;
    bool hasHighResTimer;
};

/**
 * @brief Get platform information
 */
inline PlatformInfo getPlatformInfo() {
    PlatformInfo info = {};
    
    // Platform name
#if PLATFORM_WINDOWS
    info.name = "Windows";
#elif PLATFORM_MACOS
    info.name = "macOS";
#elif PLATFORM_LINUX
    info.name = "Linux";
#else
    info.name = "Unknown";
#endif

    // Architecture
#if ARCH_X86_64
    info.architecture = "x86_64";
#elif ARCH_ARM64
    info.architecture = "ARM64";
#elif defined(ARCH_X86_32)
    info.architecture = "x86_32";
#else
    info.architecture = "Unknown";
#endif

    // Compiler
#if COMPILER_GCC
    info.compiler = "GCC";
#elif COMPILER_CLANG
    info.compiler = "Clang";
#elif COMPILER_MSVC
    info.compiler = "MSVC";
#else
    info.compiler = "Unknown";
#endif

    // Features
#if defined(HAVE_SSE2) || defined(HAVE_NEON)
    info.hasSIMD = true;
#else
    info.hasSIMD = false;
#endif

    info.hasThreads = true;  // C++11 guarantees thread support
    info.hasHighResTimer = true;  // C++11 guarantees high_resolution_clock

    return info;
}

/**
 * @brief Cross-platform sleep function
 */
inline void sleep_ms(uint32_t milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/**
 * @brief Cross-platform high-resolution timestamp
 */
inline uint64_t getHighResTimestamp() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

/**
 * @brief Cross-platform CPU core count
 */
inline unsigned int getCPUCoreCount() {
    return std::thread::hardware_concurrency();
}

/**
 * @brief Platform-specific optimizations hint
 */
inline void cpuRelax() {
#if ARCH_X86_64 || defined(ARCH_X86_32)
    #if COMPILER_GCC || COMPILER_CLANG
        __builtin_ia32_pause();
    #elif COMPILER_MSVC
        _mm_pause();
    #endif
#elif ARCH_ARM64
    #if COMPILER_GCC || COMPILER_CLANG
        __asm__ __volatile__("yield" ::: "memory");
    #endif
#else
    std::this_thread::yield();
#endif
}

/**
 * @brief Memory barrier
 */
inline void memoryBarrier() {
#if COMPILER_GCC || COMPILER_CLANG
    __sync_synchronize();
#elif COMPILER_MSVC
    MemoryBarrier();
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

} // namespace platform
} // namespace infrastructure

#endif // INFRASTRUCTURE_PLATFORM_H