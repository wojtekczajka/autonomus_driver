#include "common/logger.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::logger_ = spdlog::stdout_color_mt("console");

Logger::Logger(const std::string& logFileName) {
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    // logger_ = spdlog::basic_logger_mt("my_logger", logFileName);
    Logger::logger_->set_level(spdlog::level::info);
}

void Logger::info(const std::string& message) {
    Logger::logger_->info(message);
}

void Logger::error(const std::string& message) {
    Logger::logger_->error(message);
}

void Logger::setLogLevel(spdlog::level::level_enum level) {
    Logger::logger_->set_level(level);
}

