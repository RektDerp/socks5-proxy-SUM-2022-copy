#ifndef CUSTOM_CLogger_H
#define CUSTOM_CLogger_H
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
using namespace std;
#define LOGGER Logger::GetLogger()
/**
*   Singleton Logger Class.
*/
class Logger
{
public:
    /**
    *   Logs a message
    *   @param sMessage message to be logged.
    */
    void Log(const std::string& sMessage);
    /**
    *   Variable Length Logger function
    *   @param format string for the message to be logged.
    */
    void Log(const char* format, ...);
    /**
    *   << overloaded function to Logs a message
    *   @param sMessage message to be logged.
    */
    Logger& operator<<(const string& sMessage);
    /**
    *   Funtion to create the instance of logger class.
    *   @return singleton object of Clogger class..
    */
    static Logger* GetLogger();
    
    ~Logger();  
private:
    /**
    *    Default constructor for the Logger class.
    */
    Logger();
    /**
    *   copy constructor for the Logger class.
    */
    Logger(const Logger&) {};             // copy constructor is private
    /**
    *   assignment operator for the Logger class.
    */
    Logger& operator=(const Logger&) { return *this; };  // assignment operator is private
    /**
    *   Log file name.
    **/
    static const std::string m_sFileName;
    /**
    *   Singleton logger class object pointer.
    **/
    static Logger* m_pThis;
    /**
    *   Log file stream object.
    **/
    static ofstream m_Logfile;
};
#endif