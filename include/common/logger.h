#pragma once

#include <spdlog/spdlog.h>
#include <string>
#include <chrono>
#include <filesystem>
#include <unistd.h>

class Logger {
public:
    Logger();
    static std::string generateLogFileName();
    static void createSymlink(const std::string& logFileName);

    void info(const std::string& message);
    void error(const std::string& message);
    void setLogLevel(spdlog::level::level_enum level);

protected:
    static std::shared_ptr<spdlog::logger> logger_;
};
