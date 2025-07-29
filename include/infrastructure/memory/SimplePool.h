/**
 * @file SimplePool.h
 * @brief Simple memory pool using STL containers
 * 
 * This is a simplified version that uses standard STL containers
 * instead of complex lock-free memory management.
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef INFRASTRUCTURE_MEMORY_SIMPLE_POOL_H
#define INFRASTRUCTURE_MEMORY_SIMPLE_POOL_H

#include <vector>
#include <stack>
#include <memory>
#include <mutex>
#include <type_traits>

namespace infrastructure {
namespace memory {

/**
 * @brief Simple thread-safe memory pool
 * 
 * @tparam T Type of objects to allocate
 */
template<typename T>
class SimplePool {
private:
    mutable std::mutex mutex_;
    std::stack<std::unique_ptr<T>> available_;
    std::vector<std::unique_ptr<T>> allocated_;
    size_t maxSize_;
    size_t currentlyUsed_ = 0;
    
public:
    /**
     * @brief Constructs a memory pool
     * @param maxSize Maximum number of objects to pool
     */
    explicit SimplePool(size_t maxSize = 1000) : maxSize_(maxSize) {
        // Pre-allocate some objects
        preallocate(std::min(maxSize / 4, size_t(100)));
    }
    
    /**
     * @brief Destructor
     */
    ~SimplePool() = default;
    
    // Delete copy operations
    SimplePool(const SimplePool&) = delete;
    SimplePool& operator=(const SimplePool&) = delete;
    
    // Allow move operations
    SimplePool(SimplePool&&) = default;
    SimplePool& operator=(SimplePool&&) = default;
    
    /**
     * @brief Allocates an object from the pool
     * @return Pointer to allocated object
     */
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (available_.empty()) {
            // Create new object if under limit
            if (allocated_.size() < maxSize_) {
                if constexpr (std::is_default_constructible_v<T>) {
                    allocated_.emplace_back(std::make_unique<T>());
                    currentlyUsed_++;
                    return allocated_.back().get();
                } else {
                    // Pool exhausted, allocate on heap
                    currentlyUsed_++;
                    return new T();
                }
            } else {
                // Pool exhausted, allocate on heap
                currentlyUsed_++;
                return new T();
            }
        }
        
        // Reuse from pool
        auto obj = std::move(available_.top());
        available_.pop();
        T* ptr = obj.release();
        currentlyUsed_++;
        return ptr;
    }
    
    /**
     * @brief Returns an object to the pool
     * @param ptr Pointer to object to deallocate
     */
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        currentlyUsed_--;
        
        // Check if this object belongs to our pool
        bool isPooled = false;
        for (const auto& allocated : allocated_) {
            if (allocated.get() == ptr) {
                isPooled = true;
                break;
            }
        }
        
        if (isPooled) {
            // Reset object state if needed
            if constexpr (std::is_default_constructible_v<T>) {
                *ptr = T{};  // Reset to default state
            }
            available_.emplace(ptr);
        } else {
            // Object was allocated on heap, delete it
            delete ptr;
        }
    }
    
    /**
     * @brief Pre-allocates objects in the pool
     * @param count Number of objects to pre-allocate
     */
    void preallocate(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t toAllocate = std::min(count, maxSize_ - allocated_.size());
        
        for (size_t i = 0; i < toAllocate; ++i) {
            if constexpr (std::is_default_constructible_v<T>) {
                allocated_.emplace_back(std::make_unique<T>());
                available_.emplace(allocated_.back().get());
            }
        }
    }
    
    /**
     * @brief Gets the total capacity of the pool
     */
    size_t capacity() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocated_.size();
    }
    
    /**
     * @brief Gets the number of objects currently in use
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentlyUsed_;
    }
    
    /**
     * @brief Gets the number of available objects
     */
    size_t available() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_.size();
    }
    
    /**
     * @brief Checks if the pool is empty (all objects allocated)
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_.empty();
    }
    
    /**
     * @brief Gets memory usage statistics
     */
    struct Stats {
        size_t totalObjects;     // Total objects in pool
        size_t usedObjects;      // Objects currently in use
        size_t availableObjects; // Objects available for reuse
        size_t maxCapacity;      // Maximum pool capacity
    };
    
    Stats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return Stats{
            allocated_.size(),
            currentlyUsed_,
            available_.size(),
            maxSize_
        };
    }
};

/**
 * @brief RAII wrapper for pool-allocated objects
 */
template<typename T>
class PoolPtr {
private:
    T* ptr_;
    SimplePool<T>* pool_;
    
public:
    /**
     * @brief Constructs a pool pointer
     */
    PoolPtr(T* ptr, SimplePool<T>* pool) : ptr_(ptr), pool_(pool) {}
    
    /**
     * @brief Destructor - returns object to pool
     */
    ~PoolPtr() {
        if (ptr_ && pool_) {
            pool_->deallocate(ptr_);
        }
    }
    
    // Delete copy operations
    PoolPtr(const PoolPtr&) = delete;
    PoolPtr& operator=(const PoolPtr&) = delete;
    
    // Allow move operations
    PoolPtr(PoolPtr&& other) noexcept 
        : ptr_(other.ptr_), pool_(other.pool_) {
        other.ptr_ = nullptr;
        other.pool_ = nullptr;
    }
    
    PoolPtr& operator=(PoolPtr&& other) noexcept {
        if (this != &other) {
            if (ptr_ && pool_) {
                pool_->deallocate(ptr_);
            }
            ptr_ = other.ptr_;
            pool_ = other.pool_;
            other.ptr_ = nullptr;
            other.pool_ = nullptr;
        }
        return *this;
    }
    
    /**
     * @brief Access the object
     */
    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }
    
    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }
    
    /**
     * @brief Get raw pointer
     */
    T* get() { return ptr_; }
    const T* get() const { return ptr_; }
    
    /**
     * @brief Check if valid
     */
    explicit operator bool() const { return ptr_ != nullptr; }
    
    /**
     * @brief Release ownership
     */
    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        pool_ = nullptr;
        return tmp;
    }
};

/**
 * @brief Helper function to allocate from pool with RAII
 */
template<typename T>
PoolPtr<T> allocateFromPool(SimplePool<T>& pool) {
    return PoolPtr<T>(pool.allocate(), &pool);
}

} // namespace memory
} // namespace infrastructure

#endif // INFRASTRUCTURE_MEMORY_SIMPLE_POOL_H