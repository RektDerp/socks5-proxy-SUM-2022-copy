///////////////////////////////////////////////////////////////////////////////
// @File Name:     Logger.cpp                                                //
// @Author:        Pankaj Choudhary                                          //
// @Version:       0.0.1                                                     //
// @L.M.D:         13th April 2015                                           //
// @Upated:        23rd June 2021 by Raj Laddha                              //
// @Description:   For Logging into file                                     //
//                                                                           // 
// Detail Description:                                                       //
// Implemented complete logging mechanism, Supporting multiple logging type  //
// like as file based logging, console base logging etc. It also supported   //
// for different log type.                                                   //
//                                                                           //
// Thread Safe logging mechanism. Compatible with VC++ (Windows platform)    //
// as well as G++ (Linux platform)                                           //
//                                                                           //
// Supported Log Type: ERROR, ALARM, ALWAYS, INFO, BUFFER, TRACE, DEBUG      //
//                                                                           //
// No control for ERROR, ALRAM and ALWAYS messages. These type of messages   //
// should be always captured.                                                //
//                                                                           //
// BUFFER log type should be use while logging raw buffer or raw messages    //
//                                                                           //
// Having direct interface as well as C++ Singleton inface. can use          //
// whatever interface want.                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// C++ Header File(s)
#include <iostream>
#include <cstdlib>
#include <ctime>

// Code Specific Header Files(s)
#include "Logger.h"
#include "LogConfigReader.h"

using namespace std;
using namespace CPlusPlusLogging;
using namespace cppsecrets;

Logger* Logger::m_Instance = 0;

// Log file name. File name should be change from here only
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
    if (pos + data.size() > logSize)
    {
        rollLogFiles();
    }

    lock();
    m_File << getCurrentTime() << "  " << data << endl;
    unlock();
}

void Logger::logOnConsole(std::string& data)
{
    cout << getCurrentTime() << "  " << data << endl;
}

string Logger::getCurrentTime()
{
    string currTime;
    //Current date/time based on current time
    time_t now = time(0);
    // Convert current time to string
    char buffer[80];
    ctime_s(buffer ,80, &now);
    currTime.assign(buffer);
    // Last charactor of currentTime is "\n", so remove it
    string currentTime = currTime.substr(0, currTime.size() - 1);
    return currentTime;
}

// Interface for Error Log
void Logger::error(const char* text) throw()
{
    string data;
    data.append("[ERROR]: ");
    data.append(text);

    // ERROR must be capture
    if (m_LogType == FILE_LOG && m_LogLevel)
    {
        logIntoFile(data);
    }
    else if (m_LogType == CONSOLE && m_LogLevel)
    {
        logOnConsole(data);
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

// Interface for Alarm Log 
void Logger::alarm(const char* text) throw()
{
    string data;
    data.append("[ALARM]: ");
    data.append(text);

    // ALARM must be capture
    if (m_LogType == FILE_LOG && m_LogLevel)
    {
        logIntoFile(data);
    }
    else if (m_LogType == CONSOLE && m_LogLevel)
    {
        logOnConsole(data);
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

// Interface for Always Log 
void Logger::always(const char* text) throw()
{
    string data;
    data.append("[ALWAYS]: ");
    data.append(text);

    // No check for ALWAYS logs
    if (m_LogType == FILE_LOG && m_LogLevel)
    {
        logIntoFile(data);
    }
    else if (m_LogType == CONSOLE && m_LogLevel)
    {
        logOnConsole(data);
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

// Interface for Buffer Log 
void Logger::buffer(const char* text) throw()
{
    // Buffer is the special case. So don't add log level
    // and timestamp in the buffer message. Just log the raw bytes.
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

// Interface for Info Log
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

// Interface for Trace Log
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

// Interface for Debug Log
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

// Interfaces to control log levels
void Logger::updateLogLevel(LogLevel logLevel)
{
    m_LogLevel = logLevel;
}

// Enable all log levels
void Logger::enaleLog()
{
    m_LogLevel = ENABLE_LOG;
}

// Disable all log levels, except error and alarm
void Logger::disableLog()
{
    m_LogLevel = DISABLE_LOG;
}

// Interfaces to control log Types
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

// Interfaces to control roll over mechanism
void Logger::updateMaxLogFiles(const size_t maxFiles)
{
    if (maxFiles > 0)
        maxLogFiles = maxFiles;

    else
        maxLogFiles = MAX_LOG_FILES;

}

void Logger::updateLogSize(const size_t size)
{
    if (size > 0)
        logSize = size;

    else
        logSize = LOG_FILE_SIZE;
}

// Handle roll over mechanism
void Logger::rollLogFiles()
{
    m_File.close();

    if (maxLogFiles > 1)
    {
        string logFile = logFileName.substr(0, logFileName.length() - 4); // removing ".log" from file name

        // To check if the maximum files have reached
        if (logFilesCount >= maxLogFiles)
        {
            string deleteFileName = logFile + "_" + to_string(maxLogFiles - 1) + ".tar.gz";
            remove(deleteFileName.c_str());

            logFilesCount--;
        }

        // Renaming the files 
        for (int i = logFilesCount; i >= 2; --i)
        {
            string oldLogFileName = logFile + "_" + to_string(i - 1) + ".tar.gz";
            string newLogFileName = logFile + "_" + to_string(i) + ".tar.gz";

            rename(oldLogFileName.c_str(), newLogFileName.c_str());
        }

        string cmd = "tar -cf " + logFile + "_1.tar.gz " + logFileName;

        system(cmd.c_str()); // creating tar file
    }

    remove(logFileName.c_str());

    m_File.open(logFileName.c_str(), ios::out | ios::app);

    if (logFilesCount < maxLogFiles)
    {
        logFilesCount++;
    }
} 

// For configuration
// Note: The function sets the default parameters if any paramter is incorrect or missing  
void Logger::configure()
{
    LogConfigReader* config = LogConfigReader::getInstance();

    config->parseFile();

    LogLevel logLevel;
    LogType logType;

    string logLevel_str;
    string logType_str;

    int logFiles;
    int logFileSize;

    // Configuring the log level
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

        else if (logLevel_str == "DISABLE_LOG" || logLevel_str == "6")
            logLevel = DISABLE_LOG;

        else
            logLevel = LOG_LEVEL_TRACE;
    }

    else
        logLevel = LOG_LEVEL_TRACE;


    // Configuring the log type
    if (config->getValue("log_type", logType_str))
    {
        if (logType_str == "NO_LOG" || logType_str == "1")
            logType = NO_LOG;

        else if (logType_str == "CONSOLE" || logType_str == "2")
            logType = CONSOLE;

        else
            logType = FILE_LOG;
    }

    else
        logType = FILE_LOG;



    if (!config->getValue("max_log_files", logFiles))
        logFiles = MAX_LOG_FILES;

    if (!config->getValue("log_size", logFileSize))
        logFileSize = LOG_FILE_SIZE;


    // Setting the parameters
    m_LogLevel = logLevel;
    m_LogType = logType;

    updateMaxLogFiles(logFiles);

    updateLogSize(logFileSize);

}