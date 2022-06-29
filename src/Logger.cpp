#include "Logger.h"
#include"LogUtil.h"

const string Logger::m_sFileName = "Log.txt";
Logger* Logger::m_pThis = NULL;
ofstream Logger::m_Logfile;
Logger::Logger()
{

}
Logger* Logger::GetLogger() {
    if (m_pThis == NULL) {
        m_pThis = new Logger();
        m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
    }
    return m_pThis;
}

void Logger::Log(const char* format, ...)
{
    char* sMessage = NULL;
    int nLength = 0;
    va_list args;
    va_start(args, format);
    //  Return the number of characters in the string referenced the list of arguments.
    // _vscprintf doesn't count terminating '\0' (that's why +1)
    nLength = _vscprintf(format, args) + 1;
    sMessage = new char[nLength];
    vsprintf_s(sMessage, nLength, format, args);
    //vsprintf(sMessage, format, args);
    m_Logfile << Util::CurrentDateTime() << ":\t";
    m_Logfile << sMessage << "\n";
    va_end(args);

    delete[] sMessage;
}

void Logger::Log(const string& sMessage)
{
    m_Logfile << Util::CurrentDateTime() << ":\t";
    m_Logfile << sMessage << "\n";
}

Logger& Logger::operator<<(const string& sMessage)
{
    m_Logfile << "\n" << Util::CurrentDateTime() << ":\t";
    m_Logfile << sMessage << "\n";
    return *this;
}