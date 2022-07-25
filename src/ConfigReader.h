#ifndef _CONFIG_READER_H_
#define _CONFIG_READER_H_

#include "proxy_common.h"
#include <map>
#include <fstream>
namespace proxy {
    class ConfigReader
    {
    private:
        std::map<std::string, std::string> m_ConfigSettingMap;
        std::map<std::string, std::string> m_UsersData;

        static ConfigReader* m_pInstance;

    public:
        static std::string configFilePath;
        static ConfigReader* getInstance();

        bool parseFile(std::string fileName);


        bool getValue(std::string tag, bool& value);
        bool getValue(std::string tag, int& value);
        bool getValue(std::string tag, std::string& value);

        bool hasUser(const std::string& tag, const std::string& value);

        void dumpFileValues();
        void dumpUsersValues();
    private:

        ConfigReader(std::string configFile = "config.txt");

        ~ConfigReader();

        ConfigReader(const ConfigReader& obj) {}

        void operator=(const ConfigReader& obj) {}

        std::string trim(const std::string& str, const std::string& whitespace = " \t");
        std::string reduce(const std::string& str,
            const std::string& fill = " ",
            const std::string& whitespace = " \t");

        bool fillUsers(std::ifstream& inputFile);
    };
} // namespace proxy
#endif // End of _CONFIG_READER_H_
