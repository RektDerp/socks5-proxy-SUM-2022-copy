#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cstring>

#include "Logger.h"
#include "LogConfigReader.h"

using namespace std;

Logger* Logger::m_Instance = 0;

const string logFileName = "MyLogFile.log";

Logger::Logger()
{
    m_File.open(logFileName.c_str(), ios::out | ios::app);
    configure();
    logFilesCount = 1;

    // Initialize mutex
#ifdef _WIN32
    InitializeCriticalSection(&m_Mutex);
#else
    int ret = 0;
    ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    if (ret != 0)
    {
        printf("Logger::Logger() -- Mutex attribute not initialize!!\n");
        exit(0);
    }
    ret = pthread_mutex_init(&m_Mutex, &m_Attr);
    if (ret != 0)
    {
        printf("Logger::Logger() -- Mutex not initialize!!\n");
        exit(0);
    }
#endif
}

Logger::~Logger()
{
    m_File.close();
    if (deley != NULL)
    {
        delete deley;
    }
#ifdef _WIN32
    DeleteCriticalSection(&m_Mutex);
#else
    pthread_mutexattr_destroy(&m_Attr);
    pthread_mutex_destroy(&m_Mutex);
#endif
}

Logger* Logger::getInstance() throw ()
{
    if (m_Instance == 0)
    {
        m_Instance = new Logger();
    }
    return m_Instance;
}

void Logger::console(const char* text) throw()
{
    if ((m_LogType == CONSOLE || m_LogType == ALL_LOG) && m_LogLevel)
    {
        cout << text << endl;
    }
}

void Logger::console(std::string& text) throw()
{
    console(text.data());
}

void Logger::console(std::ostringstream& stream) throw()
{
    string text = stream.str();
    console(text.data());
}

void Logger::lock()
{
#ifdef _WIN32
    EnterCriticalSection(&m_Mutex);
#else
    pthread_mutex_lock(&m_Mutex);
#endif
}

void Logger::unlock()
{
#ifdef _WIN32
    LeaveCriticalSection(&m_Mutex);
#else
    pthread_mutex_unlock(&m_Mutex);
#endif
}

void Logger::logIntoFile(std::string& data)
{
    unsigned long pos = m_File.tellp();

    lock();
    m_File << getCurrentTime() << "  " << data << endl;
    unlock();
}

void Logger::logOnConsole(std::string& data)
{
    //lock();
    cout << data << endl;
    //unlock();
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
        logOnConsole(data);
        logIntoFile(data);
    }
}

void Logger::error(std::string& text) throw()
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
        logOnConsole(data);
        logIntoFile(data);
    }
}

void Logger::alarm(std::string& text) throw()
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
        logOnConsole(data);
        logIntoFile(data);
    }
}

void Logger::always(std::string& text) throw()
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
    if ((m_LogType == FILE_LOG) && (m_LogLevel <= LOG_LEVEL_BUFFER))
    {
        lock();
        m_File << text << endl;
        unlock();
    }
    else if ((m_LogType == CONSOLE) && (m_LogLevel <= LOG_LEVEL_BUFFER))
    {
        cout << text << endl;
    }
    else if (m_LogType == ALL_LOG && (m_LogLevel <= LOG_LEVEL_BUFFER))
    {
        lock();
        m_File << text << endl;
        cout << text << endl;
        unlock();
    }
}

void Logger::buffer(std::string& text) throw()
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

    if ((m_LogType == FILE_LOG) && (m_LogLevel <= LOG_LEVEL_INFO))
    {
        logIntoFile(data);
    }
    else if ((m_LogType == CONSOLE) && (m_LogLevel <= LOG_LEVEL_INFO))
    {
        logOnConsole(data);
    }
    else if (m_LogType == ALL_LOG && (m_LogLevel <= LOG_LEVEL_INFO))
    {
        logOnConsole(data);
        logIntoFile(data);
    }
}

void Logger::info(std::string& text) throw()
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

    if ((m_LogType == FILE_LOG) && (m_LogLevel <= LOG_LEVEL_TRACE))
    {
        logIntoFile(data);
    }
    else if ((m_LogType == CONSOLE) && (m_LogLevel <= LOG_LEVEL_TRACE))
    {
        logOnConsole(data);
    }
    else if (m_LogType == ALL_LOG && (m_LogLevel <= LOG_LEVEL_TRACE))
    {
        logOnConsole(data);
        logIntoFile(data);
    }
}

void Logger::trace(std::string& text) throw()
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

    if ((m_LogType == FILE_LOG) && (m_LogLevel <= LOG_LEVEL_DEBUG))
    {
        logIntoFile(data);
    }
    else if ((m_LogType == CONSOLE) && (m_LogLevel <= LOG_LEVEL_DEBUG))
    {
        logOnConsole(data);
    }
    else if ((m_LogType == ALL_LOG) && (m_LogLevel <= LOG_LEVEL_DEBUG))
    {
        logOnConsole(data);
        logIntoFile(data);
    }
}

void Logger::debug(std::string& text) throw()
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
    LogConfigReader* config = LogConfigReader::getInstance();

    LogLevel logLevel;
    LogType logType;

    string logLevel_str;
    string logType_str;

    if (config->getValue("log_level", logLevel_str))
    {
        if (logLevel_str == "ENABLE_LOG" || logLevel_str == "1")
            logLevel = ENABLE_LOG;

        else if (logLevel_str == "LOG_LEVEL_INFO" || logLevel_str == "2")
            logLevel = LOG_LEVEL_INFO;

        else if (logLevel_str == "LOG_LEVEL_BUFFER" || logLevel_str == "3")
            logLevel = LOG_LEVEL_BUFFER;

        else if (logLevel_str == "LOG_LEVEL_TRACE" || logLevel_str == "4")
            logLevel = LOG_LEVEL_TRACE;

        else if (logLevel_str == "LOG_LEVEL_DEBUG" || logLevel_str == "5")
            logLevel = LOG_LEVEL_DEBUG;

        else if (logLevel_str == "DISABLE_LOG" || logLevel_str == "0")
            logLevel = DISABLE_LOG;

        else
            logLevel = ENABLE_LOG;
    }
    else
        logLevel = ENABLE_LOG;

    if (config->getValue("log_type", logType_str))
    {
        if (logType_str == "NO_LOG" || logType_str == "1")
            logType = NO_LOG;

        else if (logType_str == "CONSOLE" || logType_str == "2")
            logType = CONSOLE;
        else if (logType_str == "FILE_LOG" || logType_str == "3")
            logType = FILE_LOG;
        else
            logType = ALL_LOG;
    }
    else
        logType = ALL_LOG;

    m_LogLevel = logLevel;
    m_LogType = logType;

}


