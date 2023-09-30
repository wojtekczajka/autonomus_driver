#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <variant>

class ConfigParser {
   public:
    ConfigParser(const std::string& filename);

    bool loadConfig();
    void printConfig() const;

    template <typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        auto it = configMap.find(key);
        if (it != configMap.end()) {
            try {
                return std::get<T>(it->second);
            } catch (const std::bad_variant_access&) {
                std::cerr << "Error: Incorrect type for key '" << key << "'" << std::endl;
            }
        }
        return defaultValue;
    }

   private:
    void parseLine(const std::string& line);
    std::string removeSpaces(const std::string& str);

    std::ifstream configFile;
    std::map<std::string, std::variant<std::string, int, double>> configMap;
};