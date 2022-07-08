// C++ Header File(s)
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <boost/asio.hpp>
#ifdef _WIN32
// Win Socket Header File(s)
#include <windows.h>
#include <process.h>
#else
// POSIX Socket Header File(s)
#include <errno.h>
#include <pthread.h>
#endif

//#define log_stat(dst, bnd, size, where)    StatWriter::getInstanse()->log(dst, bnd, size, where);

enum Where
{
    TO_SERVER,
    TO_CLIENT
};
class StatWriter
{
private:
    struct CSV_ROW
    {
        std::string time;
        std::string dst;
        std::string bnd;
        unsigned long long send;
        unsigned long long reqst;
        std::string timeEnd;
    };
public:
    static StatWriter* getInstanse();

    void log(std::string dst, std::string bnd, unsigned int size, Where where);
private:
    void add(std::string dst, std::string bnd, unsigned int size = 0, Where where = TO_CLIENT);
    int index_of(std::string dst, std::string bnd);
    void update(int pos, int nuberOfBytes, Where where);
    std::string getCurrentTime();
    void lock();
    void unlock();
    void conf();
    CSV_ROW getRow(std::string& s);
private:
    StatWriter();
    ~StatWriter();
    static StatWriter* _writer;

    std::string filename = "stat.csv";
    time_t _time = time(0);
    size_t size;
#ifdef	_WIN32
    CRITICAL_SECTION        m_Mutex;
#else
    pthread_mutexattr_t     m_Attr;
    pthread_mutex_t         m_Mutex;
#endif
};

inline void log_stat(boost::asio::ip::tcp::endpoint dst, boost::asio::ip::tcp::endpoint bnd, unsigned int size, Where where)
{
    std::stringstream dst_raw;
    std::stringstream bnd_raw;
    dst_raw << dst;
    bnd_raw << bnd;
    StatWriter::getInstanse()->log(dst_raw.str(), bnd_raw.str(), size, where);
}

// Создавать новый фалй статистики или все в один
// можно ли ограничиться лонг интом 
