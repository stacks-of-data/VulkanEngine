#include "Logger.hpp"
#include <ostream>

Logger::Logger()
{
	this->m_logFile.open("log.txt", std::fstream::out);
	if (!this->m_logFile.is_open())
		std::cerr << "Logger [ERROR]: Could not open log.txt for logging" << std::endl;
	this->m_logFile << "Logger [INFO]: Logger Started" << std::endl;
}

Logger::~Logger()
{
}

Logger& Logger::getInstance()
{
	static Logger logger;
	return logger;
}

static const char* levelToStr(LogLevel level)
{
	switch (level)
	{
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

void Logger::log(LogLevel level, const std::string &msg)
{
	std::string output = "Logger: ";
	output.push_back('[');
	output.append(levelToStr(level));
	output.append("]: ");
	output.append(msg);
	if (level > INFO)
		std::cerr << output << std::endl;
	else
		std::cout << output << std::endl;
	if (this->m_logFile.is_open())
	{
		this->m_logFile << output << std::endl;
		if (!this->m_logFile.good())
		{
			std::cerr << "Logger [ERROR]: Writing to log file failed" << std::endl;
			this->m_logFile.close();
		}
	}
}

void Logger::verbose(const std::string& msg) { return this->log(LogLevel::VERBOSE, msg); }
void Logger::info(const std::string& msg) { return this->log(LogLevel::INFO, msg); }
void Logger::warning(const std::string& msg) { return this->log(LogLevel::WARNING, msg); }
void Logger::error(const std::string& msg) { return this->log(LogLevel::ERROR, msg); }