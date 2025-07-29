/**
 * @file Message.cpp
 * @brief Simple FIX message implementation
 */

#include "fix/Message.h"

namespace fix {

void Message::setField(int tag, const std::string& value) {
    fields_[tag] = value;
}

std::string Message::getField(int tag) const {
    auto it = fields_.find(tag);
    return (it != fields_.end()) ? it->second : "";
}

bool Message::hasField(int tag) const {
    return fields_.find(tag) != fields_.end();
}

void Message::removeField(int tag) {
    fields_.erase(tag);
}

void Message::clear() {
    fields_.clear();
}

const std::unordered_map<int, std::string>& Message::getFields() const {
    return fields_;
}

} // namespace fix