#include "common/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <sstream>
#include <iomanip>

std::shared_ptr<spdlog::logger> Logger::logger_;

Logger::Logger() {
    std::string logFileName = generateLogFileName();
    createSymlink(logFileName);

    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    logger_ = spdlog::basic_logger_mt("my_logger", logFileName);
    logger_->set_level(spdlog::level::info);
}

std::string Logger::generateLogFileName() {
    std::string logDir = "logs";
    std::filesystem::create_directory(logDir);

    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&timeT);
    std::stringstream ss;
    ss << logDir << "/log_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
    return ss.str();
}

void Logger::createSymlink(const std::string& logFileName) {
    std::string symlinkPath = "LATEST_LOG_FILE";
    unlink(symlinkPath.c_str());
    if (symlink(logFileName.c_str(), symlinkPath.c_str())) {
        perror("Failed to create symlink for log file");
    }
}

void Logger::info(const std::string& message) {
    logger_->info(message);
}

void Logger::error(const std::string& message) {
    logger_->error(message);
}

void Logger::setLogLevel(spdlog::level::level_enum level) {
    logger_->set_level(level);
}
