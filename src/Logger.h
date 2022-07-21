#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <errno.h>
#include <pthread.h>
#endif

#define LOG_CONSOLE(x)    Logger::getInstance()->console(x)
#define LOG_ERROR(x)    Logger::getInstance()->error(x)
#define LOG_ALARM(x)	   Logger::getInstance()->alarm(x)
#define LOG_ALWAYS(x)	Logger::getInstance()->always(x)
#define LOG_INFO(x)     Logger::getInstance()->info(x)
#define LOG_BUFFER(x)   Logger::getInstance()->buffer(x)
#define LOG_TRACE(x)    Logger::getInstance()->trace(x)
#define LOG_DEBUG(x)    Logger::getInstance()->debug(x)

    typedef enum LOG_LEVEL
    {
        DISABLE_LOG = 0,  
        ENABLE_LOG = 1,
        LOG_LEVEL_INFO = 2,
        LOG_LEVEL_BUFFER = 3,
        LOG_LEVEL_TRACE = 4,
        LOG_LEVEL_DEBUG = 5,
        
    } LogLevel;

    typedef enum LOG_TYPE
    {
        NO_LOG = 1,
        CONSOLE = 2,
        FILE_LOG = 3,
        ALL_LOG = 4,
    }LogType;

    class Logger
    {
    public:
        static Logger* getInstance() throw ();

        void console(const char* text) throw();
        void console(const std::string& text) throw();
        void console(std::ostringstream& stream) throw();

        void error(const char* text) throw();
        void error(const std::string& text) throw();
        void error(std::ostringstream& stream) throw();

        void alarm(const char* text) throw();
        void alarm(const std::string& text) throw();
        void alarm(std::ostringstream& stream) throw();

        void always(const char* text) throw();
        void always(const std::string& text) throw();
        void always(std::ostringstream& stream) throw();

        void buffer(const char* text) throw();
        void buffer(const std::string& text) throw();
        void buffer(std::ostringstream& stream) throw();

        void info(const char* text) throw();
        void info(const std::string& text) throw();
        void info(std::ostringstream& stream) throw();

        void trace(const char* text) throw();
        void trace(const std::string& text) throw();
        void trace(std::ostringstream& stream) throw();

        void debug(const char* text) throw();
        void debug(const std::string& text) throw();
        void debug(std::ostringstream& stream) throw();

        void updateLogLevel(LogLevel logLevel);
        void enaleLog(); 
        void disableLog();

        void updateLogType(LogType logType);
        void enableConsoleLogging();
        void enableFileLogging();
        void enableALLLogging();


    protected:
        Logger();
        ~Logger();

        void lock();
        void unlock();

        std::string getCurrentTime();

    private:
        void logIntoFile(std::string& data);
        void logOnConsole(std::string& data);
        Logger(const Logger& obj) {}
        void operator=(const Logger& obj) {}
        void configure();

    private:
        static Logger* m_Instance;
        std::ofstream           m_File;

#ifdef	_WIN32
        CRITICAL_SECTION        m_Mutex;
#else
        pthread_mutexattr_t     m_Attr;
        pthread_mutex_t         m_Mutex;
#endif

        LogLevel                m_LogLevel;
        LogType                 m_LogType;


    };
    struct BUFF {
        std::ostringstream ss;
        std::string type;
        BUFF() = delete;
        BUFF(const std::string& type);
        BUFF(std::string&& type);
        BUFF(const BUFF&) = delete;
        BUFF& operator=(const BUFF&) = delete;
        BUFF& operator=(BUFF&&) = delete;
        BUFF(BUFF&& buf);
        template <typename T>
        BUFF& operator<<(T&& message)
        {
            ss << std::forward<T>(message);
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

    BUFF log(const std::string& type);
    BUFF log(std::string&& type);

    
#endif

    
