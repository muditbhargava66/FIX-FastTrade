#ifndef FIX_MESSAGE_H
#define FIX_MESSAGE_H

#include <string>
#include <unordered_map>

namespace fix {

class Message {
public:
    void setField(int tag, const std::string& value);
    std::string getField(int tag) const;
    bool hasField(int tag) const;
    void removeField(int tag);

    const std::unordered_map<int, std::string>& getFields() const;

private:
    std::unordered_map<int, std::string> fields_;
};

}  // namespace fix

#endif  // FIX_MESSAGE_H