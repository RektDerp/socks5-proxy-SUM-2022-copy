#include "statWriter.h"

StatWriter* StatWriter::_writer = nullptr;

StatWriter::StatWriter() : size(0)
{
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
    conf();
}
void StatWriter::log(std::string dst, std::string bnd, unsigned int size, Where where)
{
    int pos = index_of(dst, bnd);
    add(dst, bnd, size, where);
}

void StatWriter::add(std::string dst, std::string bnd, unsigned int size, Where where)
{
    std::ofstream _fout;
    _fout.open(filename.c_str(), std::ios::out | std::ios::app);
    if (_fout.is_open())
    {
        if (where == TO_CLIENT)
        {
            lock();
            _fout << "\n" << getCurrentTime() << ";" << dst << ";" << bnd << ";" << "0" << ";" << size;
            unlock();
        }
        if (where == TO_SERVER)
        {
            lock();
            _fout << "\n" << getCurrentTime() << ";" << dst << ";" << bnd << ";" << size << ";" << "0";
            unlock();
        }
    }
    _fout.close();
}

int StatWriter::index_of(std::string dst, std::string bnd)
{
    std::ifstream _fin;
    _fin.open(filename.c_str(), std::ios::in);
    int count = 0;
    std::string tmp;
    std::getline(_fin, tmp);
    while (!_fin.eof())
    {
        tmp.clear();
        std::getline(_fin, tmp);
        CSV_ROW row = getRow(tmp);
        if (row.dst == dst && row.bnd == bnd)
        {
            _fin.close();
            return count;
        }
        count++;
    }
    _fin.close();
    return -1;
}

void StatWriter::update(int pos, int numberOfBytes, Where where)
{
    std::ifstream _fin;
    _fin.open(filename.c_str(), std::ios::in);
    std::ofstream _fout;
    _fout.open("tmp.csv", std::ios::out);
    _fout << "TIME;DST;BND;SEND;REQST";
    int count = 0;
    std::string tmp;
    std::getline(_fin, tmp);
    while (!_fin.eof())
    {
        tmp.clear();
        std::getline(_fin, tmp);
        if (tmp.substr(0, 4) == "TIME")
        {
            continue;
        }
        CSV_ROW row = getRow(tmp);
        if (count == pos)
        {
            if (where == TO_CLIENT)
            {
                row.reqst += numberOfBytes;
            }
            if (where == TO_SERVER)
            {
                row.send += numberOfBytes;
            }
            row.timeEnd = getCurrentTime();
        }
        lock();
        _fout << "\n" << row.time << ";" << row.dst << ";" << row.bnd << ";" << row.send << ";" << row.reqst;
        unlock();
        //std::cout << row.time << ";" << row.dst << ";" << row.bnd << ";" << row.send << ";" << row.reqst << ";" << row.timeEnd;
        if (_fin.eof())
        {
            break;
        }
        count++;
    }
    _fin.close();
    _fout.close();
    remove(filename.c_str());
    rename("tmp.csv", filename.c_str());
}
//todo can crash the program if the number of bytes in one of the cells is greater than max unsignet long int
StatWriter::CSV_ROW StatWriter::getRow(std::string& tmp)
{
    CSV_ROW row;
    std::stringstream s(tmp);
    std::getline(s, row.time, ';');
    std::getline(s, row.dst, ';');
    std::getline(s, row.bnd, ';');
    std::string tmpForBytes;
    std::getline(s, tmpForBytes, ';');
    row.send = (tmpForBytes == "") ? 0 : stoi(tmpForBytes);
    tmpForBytes.clear();
    std::getline(s, tmpForBytes, ';');
    row.reqst = (tmpForBytes == "") ? 0 : stoi(tmpForBytes);
    tmpForBytes.clear();
    return row;
}

std::string StatWriter::getCurrentTime()
{
    _time = time(0);
    char tmp[20];
    std::strftime(tmp, 20, "%Y-%m-%d %H:%M:%S", localtime(&_time));
    return std::string(tmp);
}

StatWriter* StatWriter::getInstanse()
{
    if (_writer == 0)
    {
        _writer = new StatWriter();
    }
    return _writer;
}

void StatWriter::conf()
{
    std::ofstream _fout;
    std::ifstream _fin;
    _fin.open(filename.c_str());
    std::string tmp;
    std::getline(_fin, tmp);
    _fin.close();
    _fout.open(filename.c_str(), std::ios::out | std::ios::app);
    _fout.seekp(0);
    if (tmp.substr(0, 3) != "TIME")
    {
        _fout << "TIME;DST;BND;SEND;REQST";
    }
    _fout.close();
}

StatWriter::~StatWriter()
{
}

void StatWriter::lock()
{
#ifdef _WIN32
    EnterCriticalSection(&m_Mutex);
#else
    pthread_mutex_lock(&m_Mutex);
#endif
}

void StatWriter::unlock()
{
#ifdef _WIN32
    LeaveCriticalSection(&m_Mutex);
#else
    pthread_mutex_unlock(&m_Mutex);
#endif
}
