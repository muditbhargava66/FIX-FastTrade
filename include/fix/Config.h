#ifndef FIX_CONFIG_H
#define FIX_CONFIG_H

#include <string>
#include <unordered_map>

namespace fix {

class Config {
public:
    explicit Config(const std::string& configFilePath);

    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;

private:
    void loadFromFile(const std::string& configFilePath);
    std::string trim(const std::string& str);

    std::unordered_map<std::string, std::string> settings_;
};

}  // namespace fix

#endif  // FIX_CONFIG_H