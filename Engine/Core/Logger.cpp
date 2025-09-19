#include "Logger.hpp"
#include <exception>
#include <ostream>

Logger::Logger()
{
    this->logFile.open("log.txt", std::fstream::out);
    if (!this->logFile.is_open())
        std::cerr << "Logger [ERROR]: Could not open log.txt for logging" << std::endl;
    this->logFile << "Logger [INFO]: Logger Started" << std::endl;
}

Logger::~Logger() { }

Logger& Logger::getInstance() noexcept
{
    static Logger logger;
    return logger;
}

static const char* levelToStr(LogLevel level) noexcept
{
    switch (level) {
    case VERBOSE:
        return "VERBOSE";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& msg) noexcept
{
    try {
        std::string output = "Logger: ";
        output.push_back('[');
        output.append(levelToStr(level));
        output.append("]: ");
        output.append(msg);
        if (level > INFO)
            std::cerr << output << std::endl;
        else
            std::cout << output << std::endl;
        if (this->logFile.is_open()) {
            this->logFile << output << std::endl;
            if (!this->logFile.good()) {
                std::cerr << "Logger [ERROR]: Writing to log file failed" << std::endl;
                this->logFile.close();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Logger [ERROR]: Logging failed: " << e.what() << std::endl;
    }
}

void Logger::verbose(const std::string& msg) noexcept { log(LogLevel::VERBOSE, msg); }
void Logger::info(const std::string& msg) noexcept { log(LogLevel::INFO, msg); }
void Logger::warning(const std::string& msg) noexcept { log(LogLevel::WARNING, msg); }
void Logger::error(const std::string& msg) noexcept { log(LogLevel::ERROR, msg); }