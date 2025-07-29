#ifndef BENCHMARK_STUB_H
#define BENCHMARK_STUB_H

#include <iostream>

// Stub implementation for benchmark when Google Benchmark is not available

namespace benchmark {
    
    enum TimeUnit {
        kNanosecond,
        kMicrosecond,
        kMillisecond
    };
    
    class State {
    public:
        State() : iterations_(1000) {}
        
        class Iterator {
        public:
            Iterator(int& count) : count_(count) {}
            bool operator!=(const Iterator& /*other*/) const { return count_ > 0; }
            void operator++() { --count_; }
            int operator*() const { return count_; }
        private:
            int& count_;
        };
        
        Iterator begin() { return Iterator(iterations_); }
        Iterator end() { return Iterator(zero_); }
        
        void SetBytesProcessed(long bytes) { (void)bytes; }
        void SetItemsProcessed(long items) { (void)items; }
        long iterations() const { return 1000; }
        
    private:
        int iterations_;
        int zero_ = 0;
    };
    
    template<typename T>
    void DoNotOptimize(T&& value) {
        asm volatile("" : : "r,m"(value) : "memory");
    }
}

#define BENCHMARK(func) \
    void func(benchmark::State& state); \
    __attribute__((unused)) static void func##_stub() { \
        benchmark::State state; \
        func(state); \
    } \
    void func(benchmark::State& state)

#define BENCHMARK_MAIN() \
    int main() { \
        std::cout << "Benchmark stub - Google Benchmark not available\n"; \
        return 0; \
    }

#endif  // BENCHMARK_STUB_H