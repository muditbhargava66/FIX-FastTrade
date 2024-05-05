#include "fix/Config.h"
#include <fstream>
#include <stdexcept>

namespace fix {

Config::Config(const std::string& configFilePath) {
    loadFromFile(configFilePath);
}

void Config::loadFromFile(const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);
    if (!configFile) {
        throw std::runtime_error("Failed to open config file: " + configFilePath);
    }

    std::string line;
    while (std::getline(configFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;  // Skip empty lines and comments
        }

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid config line: " + line);
        }

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        // Trim whitespace from key and value
        key = trim(key);
        value = trim(value);

        settings_[key] = value;
    }
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        return it->second;
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception&) {
            // Fallback to default value if conversion fails
        }
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        std::string value = it->second;
        if (value == "true" || value == "1") {
            return true;
        } else if (value == "false" || value == "0") {
            return false;
        }
        // Fallback to default value if value is not recognized
    }
    return defaultValue;
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

}  // namespace fix