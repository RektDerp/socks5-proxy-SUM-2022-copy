#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cstring>

#include "Logger.h"
#include "ConfigReader.h"
namespace proxy {
    using namespace std;

    Logger* Logger::m_Instance = 0;
    std::mutex Logger::m_mutex;
    const string logFileName = "MyLogFile.log";

    Logger::Logger()
    {
        m_File.open(logFileName.c_str(), ios::out | ios::app);
        configure();
    }

    Logger::~Logger()
    {
        m_File.close();
    }

    Logger& Logger::getInstance() throw ()
    {
        if (m_Instance == 0)
        {
            m_Instance = new Logger();
        }
        return *m_Instance;
    }

    void Logger::console(const char* text) throw()
    {
        if ((m_LogType == CONSOLE || m_LogType == ALL_LOG) && m_LogLevel)
        {
            cout << text << endl;
        }
    }

    void Logger::console(const std::string& text) throw()
    {
        console(text.data());
    }

    void Logger::console(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        console(text.data());
    }
    void Logger::logIntoFile(std::string& data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_File << getCurrentTime() << "  " << data << endl;
    }

    void Logger::logOnConsole(std::string& data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        cout << data << endl;
    }

    void Logger::allLog(std::string& data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        cout << data << endl;
        m_File << getCurrentTime() << "  " << data << endl;
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

    void Logger::error(const char* text) throw()
    {
        string data;
        data.append("[ERROR]: ");
        data.append(text);
        if (m_LogType == FILE_LOG && m_LogLevel)
        {
            logIntoFile(data);
        }
        else if (m_LogType == CONSOLE && m_LogLevel)
        {
            logOnConsole(data);
        }
        else if (m_LogType == ALL_LOG && m_LogLevel)
        {
            allLog(data);
        }
    }

    void Logger::error(const std::string& text) throw()
    {
        error(text.data());
    }

    void Logger::error(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        error(text.data());
    }

    void Logger::alarm(const char* text) throw()
    {
        string data;
        data.append("[ALARM]: ");
        data.append(text);

        if (m_LogType == FILE_LOG && m_LogLevel)
        {
            logIntoFile(data);
        }
        else if (m_LogType == CONSOLE && m_LogLevel)
        {
            logOnConsole(data);
        }
        else if (m_LogType == ALL_LOG && m_LogLevel)
        {
            allLog(data);
        }
    }

    void Logger::alarm(const std::string& text) throw()
    {
        alarm(text.data());
    }

    void Logger::alarm(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        alarm(text.data());
    }

    void Logger::always(const char* text) throw()
    {
        string data;
        data.append("[ALWAYS]: ");
        data.append(text);

        if (m_LogType == FILE_LOG && m_LogLevel)
        {
            logIntoFile(data);
        }
        else if (m_LogType == CONSOLE && m_LogLevel)
        {
            logOnConsole(data);
        }
        else if (m_LogType == ALL_LOG && m_LogLevel)
        {
            allLog(data);
        }
    }

    void Logger::always(const std::string& text) throw()
    {
        always(text.data());
    }

    void Logger::always(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        always(text.data());
    }

    void Logger::buffer(const char* text) throw()
    {
        if ((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_BUFFER))
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_File << text << endl;
        }
        else if ((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_BUFFER))
        {
            cout << text << endl;
        }
        else if (m_LogType == ALL_LOG && (m_LogLevel >= LOG_LEVEL_BUFFER))
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_File << text << endl;
            cout << text << endl;
        }
    }

    void Logger::buffer(const std::string& text) throw()
    {
        buffer(text.data());
    }

    void Logger::buffer(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        buffer(text.data());
    }

    void Logger::info(const char* text) throw()
    {
        string data;
        data.append("[INFO]: ");
        data.append(text);

        if ((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_INFO))
        {
            logIntoFile(data);
        }
        else if ((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_INFO))
        {
            logOnConsole(data);
        }
        else if (m_LogType == ALL_LOG && (m_LogLevel >= LOG_LEVEL_INFO))
        {
            allLog(data);
        }
    }

    void Logger::info(const std::string& text) throw()
    {
        info(text.data());
    }

    void Logger::info(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        info(text.data());
    }

    void Logger::trace(const char* text) throw()
    {
        string data;
        data.append("[TRACE]: ");
        data.append(text);

        if ((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_TRACE))
        {
            logIntoFile(data);
        }
        else if ((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_TRACE))
        {
            logOnConsole(data);
        }
        else if (m_LogType == ALL_LOG && (m_LogLevel >= LOG_LEVEL_TRACE))
        {
            allLog(data);
        }
    }

    void Logger::trace(const std::string& text) throw()
    {
        trace(text.data());
    }

    void Logger::trace(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        trace(text.data());
    }

    void Logger::debug(const char* text) throw()
    {
        string data;
        data.append("[DEBUG]: ");
        data.append(text);

        if ((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_DEBUG))
        {
            logIntoFile(data);
        }
        else if ((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_DEBUG))
        {
            logOnConsole(data);
        }
        else if ((m_LogType == ALL_LOG) && (m_LogLevel >= LOG_LEVEL_DEBUG))
        {
            allLog(data);
        }
    }

    void Logger::debug(const std::string& text) throw()
    {
        debug(text.data());
    }

    void Logger::debug(std::ostringstream& stream) throw()
    {
        string text = stream.str();
        debug(text.data());
    }

    void Logger::updateLogLevel(LogLevel logLevel)
    {
        m_LogLevel = logLevel;
    }

    void Logger::enaleLog()
    {
        m_LogLevel = ENABLE_LOG;
    }

    void Logger::disableLog()
    {
        m_LogLevel = DISABLE_LOG;
    }

    void Logger::updateLogType(LogType logType)
    {
        m_LogType = logType;
    }

    void Logger::enableConsoleLogging()
    {
        m_LogType = CONSOLE;
    }

    void Logger::enableFileLogging()
    {
        m_LogType = FILE_LOG;
    }
    void Logger::enableALLLogging()
    {
        m_LogType = ALL_LOG;
    }

    void Logger::configure()
    {
        ConfigReader& config = ConfigReader::getInstance();
        LogLevel logLevel;
        LogType logType;

        string logLevel_str;
        string logType_str;

        if (config.getValue("log_level", logLevel_str))
        {
            if (logLevel_str == "ENABLE_LOG" || logLevel_str == "5")
                logLevel = ENABLE_LOG;

            else if (logLevel_str == "LOG_LEVEL_INFO" || logLevel_str == "1")
                logLevel = LOG_LEVEL_INFO;

            else if (logLevel_str == "LOG_LEVEL_BUFFER" || logLevel_str == "2")
                logLevel = LOG_LEVEL_BUFFER;

            else if (logLevel_str == "LOG_LEVEL_TRACE" || logLevel_str == "4")
                logLevel = LOG_LEVEL_TRACE;

            else if (logLevel_str == "LOG_LEVEL_DEBUG" || logLevel_str == "3")
                logLevel = LOG_LEVEL_DEBUG;

            else if (logLevel_str == "DISABLE_LOG" || logLevel_str == "0")
                logLevel = DISABLE_LOG;

            else
                logLevel = ENABLE_LOG;
        }
        else
            logLevel = ENABLE_LOG;

        if (config.getValue("log_type", logType_str))
        {
            if (logType_str == "NO_LOG" || logType_str == "1")
                logType = NO_LOG;

            else if (logType_str == "CONSOLE" || logType_str == "2")
                logType = CONSOLE;
            else if (logType_str == "FILE_LOG" || logType_str == "3")
                logType = FILE_LOG;
            else if (logType_str == "ALL_LOG" || logType_str == "4")
                logType = ALL_LOG;
            else
                logType = CONSOLE;
        }
        else
            logType = CONSOLE;

        m_LogLevel = logLevel;
        m_LogType = logType;
    }

    /////////////////////////////



    BUFF log(const LoggedLevel type)
    {
        return BUFF(type);
    }


    template <typename T>
    BUFF operator<<(Logger& simpleLogger, T&& message)
    {
        BUFF buf;
        buf.ss << std::forward<T>(message);
        return buf;
    }

    BUFF::BUFF(const LoggedLevel type) : type(type)
    {
    }

    BUFF::BUFF(BUFF&& buf) : ss(move(buf.ss))
    {
    }

    BUFF::~BUFF()
    {
        if (type == LoggedLevel::CONSOLE_LOG)
        {
            Logger::getInstance().console(ss);
        }
        else if (type == LoggedLevel::ERROR_LOG)
        {
            Logger::getInstance().error(ss);
        }
        else if (type == LoggedLevel::ALARM_LOG)
        {
            Logger::getInstance().alarm(ss);
        }
        else if (type == LoggedLevel::ALWAYS_LOG)
        {
            Logger::getInstance().always(ss);
        }
        else if (type == LoggedLevel::BUFFER_LOG)
        {
            Logger::getInstance().buffer(ss);
        }
        else if (type == LoggedLevel::INFO_LOG)
        {
            Logger::getInstance().info(ss);
        }
        else if (type == LoggedLevel::TRACE_LOG)
        {
            Logger::getInstance().trace(ss);
        }
        else if (type == LoggedLevel::DEBUG_LOG)
        {
            Logger::getInstance().debug(ss);
        }
    }
} // namespace proxy
