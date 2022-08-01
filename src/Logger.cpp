#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cstring>
#include <boost/algorithm/string.hpp>

#include "Logger.h"
#include "ConfigReader.h"
char logFileName [256];

namespace proxy {
    using namespace std;

    Logger* Logger::m_Instance = 0;

    std::vector<std::string> Logger::LOG_LEVEL_PREFIX = {
        " [FATAL] ",
        " [ERROR] ",
        " [ INFO] ",
        " [DEBUG] ",
        " [TRACE] "
    };

    std::vector<std::string> Logger::LOG_LEVEL_NAMES = {
       "OFF",
       "FATAL",
       "ERROR",
       "INFO",
       "DEBUG",
       "TRACE",
       "ALL"
    };

    std::vector<std::string> Logger::LOG_TYPE_NAMES = {
       "DISABLE",
       "CONSOLE",
       "FILE",
       "ENABLE",
    };

    Logger::Logger() : m_LogLevel(ALL_LOG), m_LogType(ENABLE_LOG), m_worker(1)
    {
        m_File.open(logFileName, ios::out | ios::app);
        configure();
    }

    Logger::~Logger()
    {
        m_File.close();
    }

    Logger& Logger::getInstance()
    {
        if (m_Instance == 0)
        {
            m_Instance = new Logger();
        }
        return *m_Instance;
    }

    void Logger::log(const std::string& data, LOG_LEVEL level)
    {
        ba::post(m_worker, [&] { logTask(data, level); });
    }

    void Logger::log(std::string&& data, LOG_LEVEL level)
    {
        ba::post(m_worker, 
            [this, data_Rvalue = std::move(data), level_value = level]
            {
                logTask(std::move(data_Rvalue), level_value);
            });
    }

    void Logger::logTask(const std::string& data, LOG_LEVEL level)
    {
        if (m_LogLevel >= level && level != OFF_LOG) {
            if (m_LogType == CONSOLE_LOG)
            {
                cout << getCurrentTime() << " " << LOG_LEVEL_PREFIX[level - 1] << " " << data << endl;
            }
            else if (m_LogType == FILE_LOG)
            {
                m_File << getCurrentTime() << " " << LOG_LEVEL_PREFIX[level - 1] << " " << data << endl;
            }
            else if (m_LogType == ENABLE_LOG)
            {
                m_File << getCurrentTime() << " " << LOG_LEVEL_PREFIX[level - 1] << " " << data << endl;
                cout << getCurrentTime() << " " << LOG_LEVEL_PREFIX[level - 1] << " " << data << endl;
            }
        }
    }

    string Logger::getCurrentTime()
    {
        string currTime;
        time_t now = time(0);
        char buffer[80];
        strncpy(buffer, std::ctime(&now), 26);
        currTime.assign(buffer);
        string currentTime = currTime.substr(0, currTime.size() - 1);
        return currentTime;
    }

    void Logger::updateLogLevel(LOG_LEVEL logLevel)
    {
        m_LogLevel = logLevel;
    }

    void Logger::enableLog()
    {
        m_LogLevel = ALL_LOG;
    }

    void Logger::disableLog()
    {
        m_LogLevel = OFF_LOG; 
    }

    void Logger::updateLogType(LOG_TYPE logType)
    {
        m_LogType = logType;
    }

    void Logger::enableConsoleLogging()
    {
        if (m_LogType == OFF_LOG || m_LogType == CONSOLE_LOG) {
            m_LogType = CONSOLE_LOG;
        }
        else {
            m_LogType = ENABLE_LOG;
        }
    }

    void Logger::enableFileLogging()
    {
        m_LogType = FILE_LOG; // todo fix
    }
    void Logger::enableALLLogging()
    {
        m_LogType = ENABLE_LOG;
    }

    void Logger::configure()
    {
        ConfigReader& config = ConfigReader::getInstance();

        string logLevel_str;
        string logType_str;

        if (config.getValue("log_level", logLevel_str))
        {
            boost::to_upper(logLevel_str);
            auto it = std::find(LOG_LEVEL_NAMES.begin(), LOG_LEVEL_NAMES.end(), logLevel_str);
            if (it != LOG_LEVEL_NAMES.end()) {
                m_LogLevel = static_cast<LOG_LEVEL>(it - LOG_LEVEL_NAMES.begin());
            }
            else {
                log("Invalid log level, enabling all log", ERROR_LOG);
            }
        }
        else {
            log("No log level, enabling all log", ERROR_LOG);
        }

        if (config.getValue("log_type", logType_str))
        {
            boost::to_upper(logType_str);
            auto it = std::find(LOG_TYPE_NAMES.begin(), LOG_TYPE_NAMES.end(), logType_str);
            if (it != LOG_TYPE_NAMES.end()) {
                logType_str = static_cast<LOG_TYPE>(it - LOG_TYPE_NAMES.begin());
            }
            else {
                log("Invalid log type, enabling all log", ERROR_LOG);
            }
        }
        else {
            log("No log type, enabling all log", ERROR_LOG);
        }
    }

    /////////////////////////////

    BUFF log(const LOG_LEVEL logLevel)
    {
        return BUFF(logLevel);
    }

    template <typename T>
    BUFF operator<<(Logger& simpleLogger, T&& message)
    {
        BUFF buf;
        buf.ss << std::forward<T>(message);
        return buf;
    }

    BUFF::BUFF(const LOG_LEVEL logLevel) : logLevel(logLevel)
    {}

    BUFF::BUFF(BUFF&& buf) : ss(move(buf.ss))
    {}

    BUFF::~BUFF()
    {
        Logger::getInstance().log(ss.str(), logLevel);
    }
} // namespace proxy
