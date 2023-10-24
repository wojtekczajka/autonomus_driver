

#include "common/config_parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

ConfigParser::ConfigParser(const std::string& filename) : configFile(filename) {
    loadConfig();
}

bool ConfigParser::loadConfig() {
    if (!configFile.is_open()) {
        std::cerr << "Error: Couldn't open config file." << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        parseLine(line);
    }

    configFile.close();
    return true;
}

void ConfigParser::parseLine(const std::string& line) {
    // std::string cleanedLine = removeSpaces(line);
    if (!line.empty() && line[0] != '#') {
        auto delimiterPos = line.find("=");
        if (delimiterPos != std::string::npos) {
            std::string name = line.substr(0, delimiterPos);
            std::string valueStr = line.substr(delimiterPos + 1);
            try {
                int intValue = std::stoi(valueStr);
                configMap[name] = intValue;
            } catch (const std::invalid_argument&) {
                try {
                    double doubleValue = std::stod(valueStr);
                    configMap[name] = doubleValue;
                } catch (const std::invalid_argument&) {
                    configMap[name] = valueStr;
                }
            }
        }
    }
}

std::string ConfigParser::removeSpaces(const std::string& str) {
    std::string result = str;
    result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
    return result;
}

void ConfigParser::printConfig() const {
    for (const auto& pair : configMap) {
        std::cout << pair.first << " = ";
        std::visit([](const auto& value) { std::cout << value; }, pair.second);
        std::cout << std::endl;
    }
}