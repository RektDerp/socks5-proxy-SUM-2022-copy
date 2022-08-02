#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "proxy_common.h"
#include <fstream>
#include <sstream>
#include <mutex>
#include <memory>
namespace proxy {
    enum LOG_LEVEL
    {
        OFF_LOG = 0,
        FATAL_LOG,
        ERROR_LOG,
        INFO_LOG,
        DEBUG_LOG,
        TRACE_LOG,
        ALL_LOG
    };

    enum LOG_TYPE
    {
        DISABLE_LOG = 0,
        CONSOLE_LOG,
        FILE_LOG,
        ENABLE_LOG
    };

    class BUFF;
    class Logger
    {
    public:
        static std::string logFileName;
        static Logger& getInstance();
        void log(const std::string& data, LOG_LEVEL level);
        void log(std::string&& data, LOG_LEVEL level);

        void updateLogLevel(LOG_LEVEL logLevel);
        void enableLog();
        void disableLog();

        void updateLogType(LOG_TYPE logType);
        void enableConsoleLogging();
        void enableFileLogging();
        void enableALLLogging();
        
    protected:
        Logger();
        ~Logger();

        std::string getCurrentTime();
    private:
        void logTask(const std::string& data, LOG_LEVEL level);
        Logger(const Logger& obj) {}
        void operator=(const Logger& obj) {}
        void configure();
    private:
        static Logger*  m_Instance;
        std::ofstream   m_File;
        LOG_LEVEL       m_LogLevel;
        LOG_TYPE        m_LogType;

        ba::thread_pool m_worker;
        static std::vector<std::string> LOG_LEVEL_PREFIX;
        static std::vector<std::string> LOG_LEVEL_NAMES;
        static std::vector<std::string> LOG_TYPE_NAMES;
    };

    class BUFF {
    private:
        std::ostringstream ss;
        LOG_LEVEL logLevel;
        BUFF() = delete;
        BUFF(const BUFF&) = delete;
        BUFF& operator=(const BUFF&) = delete;
        BUFF& operator=(BUFF&&) = delete;
    public:
        BUFF(const LOG_LEVEL logLevel);
        BUFF(BUFF&& buf);
        template <typename T>
        BUFF& operator<<(T&& message)
        {
            ss << std::forward<T>(message);
            return *this;
        }
        BUFF& operator<<(std::ostream& (*os)(std::ostream&))
        {
            ss << os;
            return *this;
        }
        ~BUFF();
    };

    template <typename T>
    BUFF operator<<(Logger& simpleLogger, T&& message);

    template <typename T>
    BUFF& operator<<(BUFF& buff, T& message)
    {
        return buff.operator<<(message);
    }

    BUFF log(const LOG_LEVEL logLevel);
}
#endif // _LOGGER_H_
