#pragma once

#include <fstream>
#include <iostream>

#define LOG_VERBOSE(msg) Logger::getInstance().verbose(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)

enum LogLevel { VERBOSE, INFO, WARNING, ERROR };

class Logger {
private:
    std::fstream logFile;
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

public:
    static Logger& getInstance() noexcept;
    void log(LogLevel level, const std::string& msg) noexcept;
    void verbose(const std::string& msg) noexcept;
    void info(const std::string& msg) noexcept;
    void warning(const std::string& msg) noexcept;
    void error(const std::string& msg) noexcept;
};