#include "LogConfigReader.h"

#include <algorithm>

using namespace std; // don't use this

LogConfigReader* LogConfigReader::m_pInstance = nullptr;
std::string LogConfigReader::configFilePath;

LogConfigReader::LogConfigReader(string configFile)
{
    m_ConfigSettingMap.clear();
    parseFile(configFile);
}

LogConfigReader::~LogConfigReader()
{
    m_ConfigSettingMap.clear();
}

LogConfigReader* LogConfigReader::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new LogConfigReader(configFilePath);
    }
    return m_pInstance;
}

bool LogConfigReader::getValue(std::string tag, bool& value)
{
    map<string, string>::iterator it;
    it = m_ConfigSettingMap.find(tag);
    if (it != m_ConfigSettingMap.end())
    {
        value = atoi((it->second).c_str());
        return true;
    }
    return false;
}
bool LogConfigReader::getValue(std::string tag, int& value)
{
    map<string, string>::iterator it;
    it = m_ConfigSettingMap.find(tag);
    if (it != m_ConfigSettingMap.end())
    {
        value = atoi((it->second).c_str());
        return true;
    }
    return false;
}

bool LogConfigReader::getValue(std::string tag, std::string& value)
{
    map<string, string>::iterator it;
    it = m_ConfigSettingMap.find(tag);
    if (it != m_ConfigSettingMap.end())
    {
        value = it->second;
        return true;
    }
    return false;
}
bool LogConfigReader::hasUser(const std::string& tag, const std::string& value)
{
    map<string, string>::iterator it;
    it = m_UsersData.find(tag);
    if (it != m_UsersData.end() && it->second == value)
    {
        return true;
    }
    return false;
}

bool LogConfigReader::parseFile(string fileName)
{
    ifstream inputFile;
    inputFile.open(fileName.c_str());
    string delimeter = "=";
    int initPos = 0;

    if (inputFile.fail())
    {
        cout << "Unable to find defaultConfig file" << endl;

        return false;
    }

    string line;
    while (getline(inputFile, line))
    {
        size_t found = line.find_first_of('#'); 
        string configData = line.substr(0, found);

        configData.erase(std::remove(configData.begin(), configData.end(), '\r'), configData.end());

        if (configData.empty())
            continue;

        if (configData.find('{') != string::npos)
        {
            if (!fillUsers(inputFile))
            {
                std::cout << "Users not loaded from config" << std::endl;
            }
        }
        unsigned int length = configData.find(delimeter);

        string tag, value;

        if (length != string::npos)
        {
            tag = configData.substr(initPos, length);
            value = configData.substr(length + 1);
        }

        tag = reduce(tag);
        value = reduce(value);
        if (tag.empty() || value.empty())
            continue;

        std::map<std::string, std::string>::iterator itr = m_ConfigSettingMap.find(tag);
        if (itr != m_ConfigSettingMap.end())
        {
            m_ConfigSettingMap.erase(tag);
        }

        m_ConfigSettingMap.insert(std::pair<string, string>(tag, value));
    }
    return true;
}
bool LogConfigReader::fillUsers(ifstream &inputFile)
{
    string delimeter = ":";
    int initPos = 0;

    bool work = true;
    string line1;
    while (work)
    {
        if (!getline(inputFile, line1))
        {
            work = false;
            continue;
        }
        size_t found1 = line1.find_first_of('#');
        string configData1 = line1.substr(0, found1);

        configData1.erase(std::remove(configData1.begin(), configData1.end(), '\r'), configData1.end());
        
        if (configData1.empty())
            continue;
        if (configData1.find('}') != string::npos)
        {
            return true;
        }
        unsigned int length1 = configData1.find(delimeter);
        string tag1, value1;
        if (length1 != string::npos)
        {
            tag1 = configData1.substr(initPos, length1);
            value1 = configData1.substr(length1 + 1);
        }

        tag1 = reduce(tag1);
        value1 = reduce(value1);

        if (tag1.empty() || value1.empty())
            continue;
        if (tag1 != "login")
            continue;
        string line2;
        if (!getline(inputFile, line2) || line2.find('}') != string::npos)
        {
            work = false;
            continue;
        }
        size_t found2 = line2.find_first_of('#');
        string configData2 = line2.substr(0, found2);

        configData2.erase(std::remove(configData2.begin(), configData2.end(), '\r'), configData2.end());

        if (configData2.empty())
            continue;
        if (configData2.find("}") != string::npos)
        {
            return true;
        }
        unsigned int length2 = configData2.find(delimeter);

        string tag2, value2;
        if (length2 != string::npos)
        {
            tag2 = configData2.substr(initPos, length2);
            value2 = configData2.substr(length2 + 1);
        }

        tag2 = reduce(tag2);
        value2 = reduce(value2);
        if (tag2.empty() || value2.empty())
            continue;

        std::map<std::string, std::string>::iterator itr = m_UsersData.find(value1);
        if (itr != m_UsersData.end())
        {
            m_UsersData.erase(value1);
        }
        m_UsersData.insert(std::pair<string, string>(value1, value2));
    }
    return true;
}

std::string LogConfigReader::trim(const std::string& str, const std::string& whitespace)
{
    size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    size_t strEnd = str.find_last_not_of(whitespace);
    size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string LogConfigReader::reduce(const std::string& str,
    const std::string& fill,
    const std::string& whitespace)
{
    string result = trim(str, whitespace);

    size_t beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
        size_t range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        size_t newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

void LogConfigReader::dumpFileValues()
{
    map<string, string>::iterator it;
    for (it = m_ConfigSettingMap.begin(); it != m_ConfigSettingMap.end(); ++it)
    {
        cout << it->first << " = " << it->second << endl;
    }
}
void LogConfigReader::dumpUsersValues()
{
    map<string, string>::iterator it;
    for (it = m_UsersData.begin(); it != m_UsersData.end(); ++it)
    {
        cout <<"login: \t" << it->first<< "\n" << "password: " << it->second << std::endl;
    }
}
