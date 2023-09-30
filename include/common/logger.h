#pragma once

#include <spdlog/spdlog.h>

#include <string>

class Logger {
   public:
    Logger(const std::string& logFileName);

    void info(const std::string& message);
    void error(const std::string& message);
    void setLogLevel(spdlog::level::level_enum level);

   private:
    std::shared_ptr<spdlog::logger> logger_;
};