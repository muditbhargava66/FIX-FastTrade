/**
 * @file LatencyBenchmark.cpp
 * @brief Performance benchmarks for Phase 1 optimizations
 * 
 * Measures latency improvements from:
 * - Zero-copy message parsing
 * - Memory pool allocations
 * - Lock-free queues
 * - CPU affinity
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifdef HAVE_BENCHMARK
#include <benchmark/benchmark.h>
#include <random>
#include <string>
#include <queue>
#include <mutex>
#include <memory>

#include "fix/Message.h"
#include "fix/MessageParser.h"
#include "fix/ZeroCopyMessage.h"
#include "infrastructure/memory/SimplePool.h"
#include "infrastructure/containers/SimpleQueue.h"
#include "model/Order.h"

using namespace fix;
using namespace infrastructure;

// Sample FIX message for benchmarking
static const char* SAMPLE_FIX_MESSAGE = 
    "8=FIX.4.4\0019=148\00135=D\00149=SENDER\00156=TARGET\001"
    "34=1\00152=20250101-12:00:00.123\00111=ORDER123\001"
    "21=3\00155=AAPL\00154=1\00160=20250101-12:00:00.123\001"
    "38=1000\00140=2\00144=175.50\00159=0\00110=123\001";

static const size_t SAMPLE_MESSAGE_LENGTH = strlen(SAMPLE_FIX_MESSAGE);

/**
 * @brief Benchmark traditional FIX message parsing
 */
static void BM_TraditionalFIXParsing(benchmark::State& state) {
    for (auto _ : state) {
        Message msg = MessageParser::parse(SAMPLE_FIX_MESSAGE);
        
        // Access some fields to prevent optimization
        benchmark::DoNotOptimize(msg.getField(35));
        benchmark::DoNotOptimize(msg.getField(55));
        benchmark::DoNotOptimize(msg.getField(44));
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SAMPLE_MESSAGE_LENGTH);
}

/**
 * @brief Benchmark zero-copy FIX message parsing
 */
static void BM_ZeroCopyFIXParsing(benchmark::State& state) {
    for (auto _ : state) {
        ZeroCopyMessage msg(SAMPLE_FIX_MESSAGE, SAMPLE_MESSAGE_LENGTH);
        
        // Access same fields for fair comparison
        benchmark::DoNotOptimize(msg.getField(35));
        benchmark::DoNotOptimize(msg.getField(55));
        benchmark::DoNotOptimize(msg.getField(44));
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * SAMPLE_MESSAGE_LENGTH);
}

/**
 * @brief Benchmark standard memory allocation
 */
static void BM_StandardAllocation(benchmark::State& state) {
    for (auto _ : state) {
        auto order = std::make_unique<model::Order>();
        benchmark::DoNotOptimize(order.get());
        // Order destroyed automatically
    }
    
    state.SetItemsProcessed(state.iterations());
}

/**
 * @brief Benchmark memory pool allocation
 */
static void BM_MemoryPoolAllocation(benchmark::State& state) {
    memory::SimplePool<model::Order> pool(10000);
    
    for (auto _ : state) {
        auto* order = pool.allocate();
        benchmark::DoNotOptimize(order);
        pool.deallocate(order);
    }
    
    state.SetItemsProcessed(state.iterations());
}

/**
 * @brief Benchmark standard queue (mutex-based)
 */
static void BM_StandardQueue(benchmark::State& state) {
    std::queue<int> queue;
    std::mutex mutex;
    
    for (auto _ : state) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(42);
        }
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!queue.empty()) {
                benchmark::DoNotOptimize(queue.front());
                queue.pop();
            }
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}

/**
 * @brief Benchmark lock-free queue
 */
static void BM_LockFreeQueue(benchmark::State& state) {
    containers::SimpleQueue<int> queue(10000);
    
    for (auto _ : state) {
        queue.enqueue(42);
        int value;
        if (queue.try_dequeue(value)) {
            benchmark::DoNotOptimize(value);
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}

/**
 * @brief Benchmark end-to-end order processing (traditional)
 */
static void BM_TraditionalOrderProcessing(benchmark::State& state) {
    for (auto _ : state) {
        // Parse message
        Message msg = MessageParser::parse(SAMPLE_FIX_MESSAGE);
        
        // Create order with parsed data
        std::string symbol = msg.getField(55);
        int quantity = std::stoi(msg.getField(38));
        double price = std::stod(msg.getField(44));
        
        auto order = std::make_unique<model::Order>(
            "ORDER_123", symbol, price, quantity, 
            model::Order::Side::Buy, model::Order::OrderType::Limit);
        
        // Process order (simulated)
        benchmark::DoNotOptimize(order->getSymbol());
    }
    
    state.SetItemsProcessed(state.iterations());
}

/**
 * @brief Benchmark end-to-end order processing (optimized)
 */
static void BM_OptimizedOrderProcessing(benchmark::State& state) {
    memory::SimplePool<model::Order> orderPool(1000);
    
    for (auto _ : state) {
        // Zero-copy parse
        ZeroCopyMessage msg(SAMPLE_FIX_MESSAGE, SAMPLE_MESSAGE_LENGTH);
        
        // Get fields with zero-copy views
        std::string symbol(msg.getField(55));
        
        auto qty = msg.getFieldAsInt(38);
        int quantity = qty ? static_cast<int>(*qty) : 100;
        
        auto price = msg.getFieldAsDouble(44);
        double orderPrice = price ? *price : 100.0;
        
        // Pool allocation with constructor
        auto* order = orderPool.allocate();
        new (order) model::Order("ORDER_123", symbol, orderPrice, quantity, 
                                model::Order::Side::Buy, model::Order::OrderType::Limit);
        
        // Process order (simulated)
        benchmark::DoNotOptimize(order->getSymbol());
        
        // Destroy and return to pool
        order->~Order();
        orderPool.deallocate(order);
    }
    
    state.SetItemsProcessed(state.iterations());
}

/**
 * @brief Benchmark SIMD checksum calculation
 */
static void BM_ChecksumCalculation(benchmark::State& state) {
    std::string message(1024, 'A');  // 1KB message
    
    for (auto _ : state) {
        uint8_t checksum = 0;
        for (char c : message) {
            checksum += static_cast<uint8_t>(c);
        }
        benchmark::DoNotOptimize(checksum);
    }
    
    state.SetBytesProcessed(state.iterations() * message.size());
}

// Register benchmarks
BENCHMARK(BM_TraditionalFIXParsing);
BENCHMARK(BM_ZeroCopyFIXParsing);
BENCHMARK(BM_StandardAllocation);
BENCHMARK(BM_MemoryPoolAllocation);
BENCHMARK(BM_StandardQueue);
BENCHMARK(BM_LockFreeQueue);
BENCHMARK(BM_TraditionalOrderProcessing);
BENCHMARK(BM_OptimizedOrderProcessing);
BENCHMARK(BM_ChecksumCalculation);

BENCHMARK_MAIN();

#else
// Stub implementation when Google Benchmark is not available
// This ensures the file compiles but doesn't interfere with GoogleTest

#include <iostream>

// Empty stub functions to satisfy the linker
static void benchmark_stub() {
    // This function exists only to make the file compile
    // when Google Benchmark is not available
}

#endif  // HAVE_BENCHMARK
