/**
 * @file ZeroCopyMessage.cpp
 * @brief Implementation of zero-copy FIX message parser
 * 
 * @author FIX-FastTrade Team
 * @date 2025
 */

#include "fix/ZeroCopyMessage.h"
#include <algorithm>
#include <cstring>
#include <string>

namespace fix {

// ZeroCopyMessage implementation

ZeroCopyMessage::ZeroCopyMessage(const char* buffer, size_t length)
    : buffer_(buffer), length_(length) {
    // Initialize quick lookup table
    quickLookup_.fill(-1);
    
    // Parse fields
    parseFields();
}

void ZeroCopyMessage::parseFields() {
    const char* current = buffer_;
    const char* end = buffer_ + length_;
    
    while (current < end && cachedFieldCount_ < MAX_CACHED_FIELDS) {
        // Find '='
        const char* equalPos = static_cast<const char*>(
            std::memchr(current, '=', end - current));
        if (!equalPos || equalPos >= end) break;
        
        // Parse tag
        int tag = 0;
        const char* tagEnd = equalPos;
        for (const char* p = current; p < tagEnd; ++p) {
            tag = tag * 10 + (*p - '0');
        }
        
        // Find SOH using platform-agnostic search
        const char* sohPos = infrastructure::simd::findByte(
            equalPos + 1, '\001', end - (equalPos + 1));
        if (!sohPos) {
            // Last field might not have SOH
            sohPos = end;
        }
        
        // Store field reference
        auto& field = cachedFields_[cachedFieldCount_];
        field.tagStart = current;
        field.valueStart = equalPos + 1;
        field.valueLength = static_cast<uint16_t>(sohPos - (equalPos + 1));
        field.tag = static_cast<uint16_t>(tag);
        
        // Update quick lookup for common tags
        if (static_cast<size_t>(tag) < QUICK_LOOKUP_SIZE) {
            quickLookup_[tag] = static_cast<int16_t>(cachedFieldCount_);
        }
        
        cachedFieldCount_++;
        current = sohPos + 1;
    }
}

const ZeroCopyMessage::FieldRef* ZeroCopyMessage::findField(int tag) const noexcept {
    // Check quick lookup first
    if (static_cast<size_t>(tag) < QUICK_LOOKUP_SIZE && quickLookup_[tag] >= 0) {
        return &cachedFields_[quickLookup_[tag]];
    }
    
    // Linear search for other tags
    for (uint32_t i = 0; i < cachedFieldCount_; ++i) {
        if (cachedFields_[i].tag == tag) {
            return &cachedFields_[i];
        }
    }
    
    return nullptr;
}

std::string_view ZeroCopyMessage::getField(int tag) const noexcept {
    const FieldRef* field = findField(tag);
    if (field) {
        return std::string_view(field->valueStart, field->valueLength);
    }
    return std::string_view();
}

std::optional<int64_t> ZeroCopyMessage::getFieldAsInt(int tag) const noexcept {
    auto field = getField(tag);
    if (field.empty()) return std::nullopt;
    
    int64_t value = 0;
    bool negative = false;
    const char* p = field.data();
    const char* end = p + field.size();
    
    // Handle negative sign
    if (*p == '-') {
        negative = true;
        ++p;
    }
    
    // Fast integer parsing
    while (p < end) {
        if (*p < '0' || *p > '9') return std::nullopt;
        value = value * 10 + (*p - '0');
        ++p;
    }
    
    return negative ? -value : value;
}

std::optional<double> ZeroCopyMessage::getFieldAsDouble(int tag) const noexcept {
    auto field = getField(tag);
    if (field.empty()) return std::nullopt;
    
    // Use strtod for double parsing since from_chars doesn't support double on all platforms
    try {
        std::string str(field.data(), field.size());
        return std::stod(str);
    } catch (...) {
        return std::nullopt;
    }
}

bool ZeroCopyMessage::hasField(int tag) const noexcept {
    return findField(tag) != nullptr;
}

std::string_view ZeroCopyMessage::getMsgType() const noexcept {
    if (!msgTypeParsed_) {
        msgType_ = getField(Tags::MsgType);
        msgTypeParsed_ = true;
    }
    return msgType_;
}

bool ZeroCopyMessage::validateChecksum() const noexcept {
    // Find checksum field
    auto checksumField = getField(Tags::CheckSum);
    if (checksumField.empty()) return false;
    
    // Calculate actual checksum
    const char* checksumStart = std::strstr(buffer_, "10=");
    if (!checksumStart) return false;
    
    size_t lengthToCheck = checksumStart - buffer_;
    uint32_t sum = infrastructure::simd::calculateChecksum(buffer_, lengthToCheck);
    uint8_t actualChecksum = sum % 256;
    
    // Parse expected checksum
    auto expectedOpt = getFieldAsInt(Tags::CheckSum);
    if (!expectedOpt) return false;
    
    return actualChecksum == static_cast<uint8_t>(*expectedOpt);
}

// ZeroCopyMessageParser implementation

const char* ZeroCopyMessageParser::findSOH(const char* start, const char* end) {
    return infrastructure::simd::findByte(start, SOH, end - start);
}

uint8_t ZeroCopyMessageParser::calculateChecksum(const char* start, size_t length) {
    uint32_t sum = infrastructure::simd::calculateChecksum(start, length);
    return static_cast<uint8_t>(sum % 256);
}

ZeroCopyMessage ZeroCopyMessageParser::parse(const char* buffer, size_t length) {
    return ZeroCopyMessage(buffer, length);
}

ZeroCopyMessage ZeroCopyMessageParser::parseWithValidation(const char* buffer, 
                                                           size_t length,
                                                           std::string& error) {
    ZeroCopyMessage msg(buffer, length);
    
    // Validate required fields
    if (!msg.hasField(Tags::BeginString)) {
        error = "Missing BeginString";
        return msg;
    }
    
    if (!msg.hasField(Tags::BodyLength)) {
        error = "Missing BodyLength";
        return msg;
    }
    
    if (!msg.hasField(Tags::MsgType)) {
        error = "Missing MsgType";
        return msg;
    }
    
    // Validate checksum
    if (!msg.validateChecksum()) {
        error = "Invalid checksum";
        return msg;
    }
    
    error.clear();
    return msg;
}

// SpecializedMessageParser implementation

SpecializedMessageParser::NewOrderSingleFields 
SpecializedMessageParser::parseNewOrderSingle(const char* buffer, size_t length) noexcept {
    NewOrderSingleFields fields;
    
    // Quick check for message type
    const char* msgTypePos = std::strstr(buffer, "35=D\001");
    if (!msgTypePos) return fields;
    
    const char* current = buffer;
    const char* end = buffer + length;
    
    // Use optimized parsing for known fields
    while (current < end) {
        // Check for specific tags we care about
        if (current[0] == '1' && current[1] == '1' && current[2] == '=') {
            // ClOrdID (11)
            const char* valueStart = current + 3;
            const char* valueEnd = ZeroCopyMessageParser::findSOH(valueStart, end);
            fields.clOrdID = std::string_view(valueStart, valueEnd - valueStart);
            current = valueEnd + 1;
        }
        else if (current[0] == '5' && current[1] == '5' && current[2] == '=') {
            // Symbol (55)
            const char* valueStart = current + 3;
            const char* valueEnd = ZeroCopyMessageParser::findSOH(valueStart, end);
            fields.symbol = std::string_view(valueStart, valueEnd - valueStart);
            current = valueEnd + 1;
        }
        else if (current[0] == '5' && current[1] == '4' && current[2] == '=') {
            // Side (54)
            fields.side = current[3];
            current = ZeroCopyMessageParser::findSOH(current + 3, end) + 1;
        }
        else if (current[0] == '3' && current[1] == '8' && current[2] == '=') {
            // OrderQty (38)
            const char* valueStart = current + 3;
            const char* valueEnd = ZeroCopyMessageParser::findSOH(valueStart, end);
            
            // Fast integer parsing
            int64_t qty = 0;
            for (const char* p = valueStart; p < valueEnd; ++p) {
                qty = qty * 10 + (*p - '0');
            }
            fields.orderQty = qty;
            current = valueEnd + 1;
        }
        else if (current[0] == '4' && current[1] == '0' && current[2] == '=') {
            // OrdType (40)
            fields.ordType = current[3];
            current = ZeroCopyMessageParser::findSOH(current + 3, end) + 1;
        }
        else if (current[0] == '4' && current[1] == '4' && current[2] == '=') {
            // Price (44)
            const char* valueStart = current + 3;
            const char* valueEnd = ZeroCopyMessageParser::findSOH(valueStart, end);
            
            // Parse price
            try {
                std::string priceStr(valueStart, valueEnd - valueStart);
                fields.price = std::stod(priceStr);
            } catch (...) {
                fields.price = 0.0;
            }
            current = valueEnd + 1;
        }
        else {
            // Skip to next field
            current = ZeroCopyMessageParser::findSOH(current, end) + 1;
        }
    }
    
    fields.valid = true;
    return fields;
}

} // namespace fix
