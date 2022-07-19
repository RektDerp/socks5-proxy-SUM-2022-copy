#ifndef _CONFIG_READER_H_
#define _CONFIG_READER_H_

#include <iostream>
#include <map>
#include <string>
#include <fstream>

    class LogConfigReader
    {
    private:
        std::map<std::string, std::string> m_ConfigSettingMap;
        std::map<std::string, std::string> m_UsersData;

        static LogConfigReader* m_pInstance;

    public:
        static std::string configFilePath;
        static LogConfigReader* getInstance();

        bool parseFile(std::string fileName);

            
        bool getValue(std::string tag, bool& value);
        bool getValue(std::string tag, int& value);
        bool getValue(std::string tag, std::string& value);

        bool hasUser(const std::string& tag, const std::string& value);

        void dumpFileValues();
        void dumpUsersValues();
    private:

        LogConfigReader(std::string configFile = "config.txt");

        ~LogConfigReader();

        LogConfigReader(const LogConfigReader& obj) {}

        void operator=(const LogConfigReader& obj) {}

        std::string trim(const std::string& str, const std::string& whitespace = " \t");
        std::string reduce(const std::string& str,
            const std::string& fill = " ",
            const std::string& whitespace = " \t");

        bool fillUsers(std::ifstream &inputFile);
    };


#endif // End of _CONFIG_READER_H_
