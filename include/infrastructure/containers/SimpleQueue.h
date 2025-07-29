/**
 * @file SimpleQueue.h
 * @brief Simple thread-safe queue using STL containers
 * 
 * This is a simplified version that uses standard STL containers
 * with mutex protection instead of lock-free algorithms.
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef INFRASTRUCTURE_CONTAINERS_SIMPLE_QUEUE_H
#define INFRASTRUCTURE_CONTAINERS_SIMPLE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>

namespace infrastructure {
namespace containers {

/**
 * @brief Thread-safe queue using STL containers
 * 
 * @tparam T Type of elements in the queue
 */
template<typename T>
class SimpleQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable condition_;
    bool closed_ = false;
    
public:
    /**
     * @brief Default constructor
     */
    SimpleQueue() = default;
    
    /**
     * @brief Constructor with capacity hint (ignored for simplicity)
     */
    explicit SimpleQueue(size_t capacity) { (void)capacity; }
    
    /**
     * @brief Destructor - closes the queue
     */
    ~SimpleQueue() {
        close();
    }
    
    // Delete copy operations
    SimpleQueue(const SimpleQueue&) = delete;
    SimpleQueue& operator=(const SimpleQueue&) = delete;
    
    // Allow move operations
    SimpleQueue(SimpleQueue&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mutex_);
        queue_ = std::move(other.queue_);
        closed_ = other.closed_;
    }
    
    SimpleQueue& operator=(SimpleQueue&& other) noexcept {
        if (this != &other) {
            std::lock(mutex_, other.mutex_);
            std::lock_guard<std::mutex> lock1(mutex_, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(other.mutex_, std::adopt_lock);
            
            queue_ = std::move(other.queue_);
            closed_ = other.closed_;
        }
        return *this;
    }
    
    /**
     * @brief Enqueues an item (by move or copy)
     * @param item Item to enqueue
     * @return true if successful, false if queue is closed
     */
    template<typename U>
    bool enqueue(U&& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closed_) return false;
        
        queue_.push(std::forward<U>(item));
        condition_.notify_one();
        return true;
    }
    
    /**
     * @brief Enqueues an item with in-place construction
     * @param args Arguments for constructing T
     * @return true if successful, false if queue is closed
     */
    template<typename... Args>
    bool emplace(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closed_) return false;
        
        queue_.emplace(std::forward<Args>(args)...);
        condition_.notify_one();
        return true;
    }
    
    /**
     * @brief Dequeues an item (blocking)
     * @return Optional containing the item, or empty if queue is closed
     */
    std::optional<T> dequeue() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        condition_.wait(lock, [this] { return !queue_.empty() || closed_; });
        
        if (queue_.empty()) {
            return std::nullopt;  // Queue was closed
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
    
    /**
     * @brief Tries to dequeue without blocking
     * @param[out] item Reference to store dequeued item
     * @return true if successful, false if queue was empty
     */
    bool try_dequeue(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.empty()) {
            return false;
        }
        
        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }
    
    /**
     * @brief Tries to dequeue with timeout
     * @param timeout Maximum time to wait
     * @return Optional containing the item, or empty if timeout/closed
     */
    template<typename Rep, typename Period>
    std::optional<T> dequeue_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (condition_.wait_for(lock, timeout, [this] { return !queue_.empty() || closed_; })) {
            if (!queue_.empty()) {
                T item = std::move(queue_.front());
                queue_.pop();
                return item;
            }
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief Checks if queue is empty
     * @return true if empty
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    /**
     * @brief Gets the size of the queue
     * @return Number of elements
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    /**
     * @brief Closes the queue
     * 
     * After closing, no new items can be enqueued and
     * all waiting dequeue operations will return empty.
     */
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        condition_.notify_all();
    }
    
    /**
     * @brief Checks if the queue is closed
     */
    bool is_closed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }
    
    /**
     * @brief Clears all items from the queue
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<T> empty;
        queue_.swap(empty);
    }
};

} // namespace containers
} // namespace infrastructure

#endif // INFRASTRUCTURE_CONTAINERS_SIMPLE_QUEUE_H