/**
 * @file ZeroCopyMessage.h
 * @brief Zero-copy FIX message representation for ultra-low latency
 * 
 * This implementation avoids all memory allocations by referencing
 * the original message buffer directly. Field access is optimized
 * using cache-friendly data structures and SIMD operations.
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#ifndef FIX_ZERO_COPY_MESSAGE_H
#define FIX_ZERO_COPY_MESSAGE_H

#include <string_view>
#include <array>
#include <optional>
#include <charconv>
#include <cstring>
#include "infrastructure/platform/Platform.h"
#include "infrastructure/simd/SIMDUtils.h"

namespace fix {

/**
 * @brief Common FIX field tags
 */
namespace Tags {
    constexpr int BeginString = 8;
    constexpr int BodyLength = 9;
    constexpr int CheckSum = 10;
    constexpr int ClOrdID = 11;
    constexpr int CumQty = 14;
    constexpr int ExecID = 17;
    constexpr int HandlInst = 21;
    constexpr int MsgSeqNum = 34;
    constexpr int MsgType = 35;
    constexpr int OrderQty = 38;
    constexpr int OrdType = 40;
    constexpr int OrigClOrdID = 41;
    constexpr int Price = 44;
    constexpr int SenderCompID = 49;
    constexpr int SendingTime = 52;
    constexpr int Side = 54;
    constexpr int Symbol = 55;
    constexpr int TargetCompID = 56;
    constexpr int Text = 58;
    constexpr int TransactTime = 60;
    constexpr int SettlDate = 64;
    constexpr int SymbolSfx = 65;
    constexpr int ExecType = 150;
    constexpr int LeavesQty = 151;
    constexpr int AvgPx = 6;
}

/**
 * @brief Zero-copy FIX message that references the original buffer
 * 
 * Features:
 * - Zero memory allocations
 * - O(1) field access for cached fields
 * - SIMD-optimized parsing
 * - Cache-line aligned for performance
 */
class alignas(64) ZeroCopyMessage {
private:
    /**
     * @brief Field reference structure
     */
    struct FieldRef {
        const char* tagStart;      // Start of tag
        const char* valueStart;    // Start of value (after '=')
        uint16_t valueLength;      // Length of value
        uint16_t tag;              // Parsed tag number
        
        FieldRef() : tagStart(nullptr), valueStart(nullptr), 
                     valueLength(0), tag(0) {}
    };
    
    // Original message buffer
    const char* buffer_;
    const size_t length_;
    
    // Pre-allocated array for common fields (no heap allocation)
    static constexpr size_t MAX_CACHED_FIELDS = 64;
    std::array<FieldRef, MAX_CACHED_FIELDS> cachedFields_;
    uint32_t cachedFieldCount_ = 0;
    
    // Quick lookup table for common tags (direct indexing)
    static constexpr size_t QUICK_LOOKUP_SIZE = 256;
    std::array<int16_t, QUICK_LOOKUP_SIZE> quickLookup_;
    
    // Parsed message type for quick access
    mutable std::string_view msgType_;
    mutable bool msgTypeParsed_ = false;
    
    /**
     * @brief Parse fields using SIMD operations
     */
    void parseFields();
    
    /**
     * @brief Find a field using linear search
     */
    const FieldRef* findField(int tag) const noexcept;
    
public:
    /**
     * @brief Constructs a zero-copy message from buffer
     * @param buffer Original message buffer
     * @param length Length of message
     */
    ZeroCopyMessage(const char* buffer, size_t length);
    
    /**
     * @brief Gets a field value as string_view
     * @param tag FIX field tag
     * @return Field value or empty view if not found
     */
    [[nodiscard]] std::string_view getField(int tag) const noexcept;
    
    /**
     * @brief Gets a field value as integer
     * @param tag FIX field tag
     * @return Parsed integer value or nullopt if not found/invalid
     */
    [[nodiscard]] std::optional<int64_t> getFieldAsInt(int tag) const noexcept;
    
    /**
     * @brief Gets a field value as double
     * @param tag FIX field tag
     * @return Parsed double value or nullopt if not found/invalid
     */
    [[nodiscard]] std::optional<double> getFieldAsDouble(int tag) const noexcept;
    
    /**
     * @brief Gets a field value as specific type
     * @tparam T Type to parse to
     * @param tag FIX field tag
     * @return Parsed value or nullopt if not found/invalid
     */
    template<typename T>
    [[nodiscard]] std::optional<T> getFieldAs(int tag) const noexcept {
        auto field = getField(tag);
        if (field.empty()) return std::nullopt;
        
        T value;
        auto [ptr, ec] = std::from_chars(field.data(), 
                                         field.data() + field.size(), 
                                         value);
        if (ec == std::errc()) {
            return value;
        }
        return std::nullopt;
    }
    
    /**
     * @brief Checks if a field exists
     * @param tag FIX field tag
     * @return true if field exists
     */
    [[nodiscard]] bool hasField(int tag) const noexcept;
    
    /**
     * @brief Gets message type (cached)
     * @return Message type field value
     */
    [[nodiscard]] std::string_view getMsgType() const noexcept;
    
    /**
     * @brief Gets the original buffer
     */
    [[nodiscard]] const char* buffer() const noexcept { return buffer_; }
    
    /**
     * @brief Gets the message length
     */
    [[nodiscard]] size_t length() const noexcept { return length_; }
    
    /**
     * @brief Gets number of cached fields
     */
    [[nodiscard]] size_t fieldCount() const noexcept { return cachedFieldCount_; }
    
    /**
     * @brief Validates checksum
     * @return true if checksum is valid
     */
    [[nodiscard]] bool validateChecksum() const noexcept;
    
    /**
     * @brief Iterator for fields
     */
    class FieldIterator {
    private:
        const ZeroCopyMessage* msg_;
        size_t index_;
        
    public:
        FieldIterator(const ZeroCopyMessage* msg, size_t index) 
            : msg_(msg), index_(index) {}
        
        bool operator!=(const FieldIterator& other) const {
            return index_ != other.index_;
        }
        
        FieldIterator& operator++() {
            ++index_;
            return *this;
        }
        
        std::pair<int, std::string_view> operator*() const {
            if (index_ < msg_->cachedFieldCount_) {
                const auto& field = msg_->cachedFields_[index_];
                return {field.tag, 
                        std::string_view(field.valueStart, field.valueLength)};
            }
            return {0, std::string_view()};
        }
    };
    
    /**
     * @brief Begin iterator
     */
    FieldIterator begin() const { return FieldIterator(this, 0); }
    
    /**
     * @brief End iterator
     */
    FieldIterator end() const { return FieldIterator(this, cachedFieldCount_); }
};

/**
 * @brief Zero-copy FIX message parser with SIMD optimizations
 * 
 * This parser creates ZeroCopyMessage instances without any
 * memory allocations, using SIMD instructions for fast parsing.
 */
class ZeroCopyMessageParser {
public:
    // SOH character (ASCII 1)
    static constexpr char SOH = '\001';
    static constexpr char EQUALS = '=';
    
    /**
     * @brief Find SOH character in buffer
     * @param start Start of search
     * @param end End of search
     * @return Pointer to SOH or end if not found
     */
    static const char* findSOH(const char* start, const char* end);
    
    /**
     * @brief Calculate FIX checksum
     * @param start Start of buffer
     * @param length Length to calculate
     * @return Checksum value
     */
    static uint8_t calculateChecksum(const char* start, size_t length);
    /**
     * @brief Parse a FIX message without allocations
     * @param buffer Message buffer
     * @param length Message length
     * @return Zero-copy message
     */
    static ZeroCopyMessage parse(const char* buffer, size_t length);
    
    /**
     * @brief Parse with validation
     * @param buffer Message buffer
     * @param length Message length
     * @param[out] error Error message if parsing fails
     * @return Zero-copy message or invalid message on error
     */
    static ZeroCopyMessage parseWithValidation(const char* buffer, 
                                               size_t length,
                                               std::string& error);
    
    /**
     * @brief Batch parse multiple messages
     * @param buffer Buffer containing multiple messages
     * @param length Total buffer length
     * @param[out] messages Vector to store parsed messages
     * @return Number of messages parsed
     */
    template<typename Container>
    static size_t parseBatch(const char* buffer, 
                            size_t length,
                            Container& messages) {
        const char* current = buffer;
        const char* end = buffer + length;
        size_t count = 0;
        
        while (current < end) {
            // Find message end (CheckSum field)
            const char* msgEnd = std::strstr(current, "10=");
            if (!msgEnd) break;
            
            // Find SOH after checksum
            msgEnd = findSOH(msgEnd, end);
            if (msgEnd >= end) break;
            
            size_t msgLen = msgEnd - current;
            messages.emplace_back(parse(current, msgLen));
            
            current = msgEnd;
            ++count;
        }
        
        return count;
    }
};

/**
 * @brief Specialized parser for common message types
 * 
 * Provides ultra-fast parsing for specific message types
 * by knowing the expected field layout in advance.
 */
class SpecializedMessageParser {
public:
    /**
     * @brief Fast parse for NewOrderSingle (35=D)
     */
    struct NewOrderSingleFields {
        std::string_view clOrdID;
        std::string_view symbol;
        char side;
        int64_t orderQty;
        char ordType;
        double price;
        std::string_view timeInForce;
        
        bool valid = false;
    };
    
    static NewOrderSingleFields parseNewOrderSingle(const char* buffer, 
                                                    size_t length) noexcept;
    
    /**
     * @brief Fast parse for ExecutionReport (35=8)
     */
    struct ExecutionReportFields {
        std::string_view orderID;
        std::string_view execID;
        char execType;
        char ordStatus;
        std::string_view symbol;
        char side;
        int64_t orderQty;
        double price;
        int64_t leavesQty;
        int64_t cumQty;
        double avgPx;
        
        bool valid = false;
    };
    
    static ExecutionReportFields parseExecutionReport(const char* buffer,
                                                     size_t length) noexcept;
};

} // namespace fix

#endif // FIX_ZERO_COPY_MESSAGE_H
