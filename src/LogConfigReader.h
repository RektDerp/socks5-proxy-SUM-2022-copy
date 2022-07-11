#ifndef _CONFIG_READER_H_
#define _CONFIG_READER_H_

// C++ Header File(s)
#include <iostream>
#include <map>
#include <string>

// Define namespace for this class. so that anyonelo
// can easily integrate it. The class name is vary generic
// so the chance to collapse the class name is high.
// So need to define the class inside the namespace.

namespace cppsecrets
{
    
    class LogConfigReader
    {
    private:

        // Define the map to store data from the config file
        std::map<std::string, std::string> m_ConfigSettingMap;

        // Static pointer instance to make this class singleton.
        static LogConfigReader* m_pInstance;

    public:
        static std::string configFilePath;
        // Public static method getInstance(). This function is
        // responsible for object creation.
        static LogConfigReader* getInstance();

        // Parse the config file.
        bool parseFile(std::string fileName);

        // Overloaded getValue() function.
        // Value of the tag in cofiguration file could be
        // string or integer. So the caller need to take care this.
        // Caller need to call appropiate function based on the
        // data type of the value of the tag.

        bool getValue(std::string tag, int& value);
        bool getValue(std::string tag, std::string& value);

        // Function dumpFileValues is for only debug purpose
        void dumpFileValues();

    private:

        // Define constructor in the private section to make this
        // class as singleton.
        LogConfigReader(std::string configFile = "config.txt");

        // Define destructor in private section, so no one can delete 
        // the instance of this class.
        ~LogConfigReader();

        // Define copy constructor in the private section, so that no one can 
        // voilate the singleton policy of this class
        LogConfigReader(const LogConfigReader& obj) {}

        // Define assignment operator in the private section, so that no one can 
        // voilate the singleton policy of this class
        void operator=(const LogConfigReader& obj) {}

        // Helper function to trim the tag and value. These helper function is
        // calling to trim the un-necessary spaces.
        std::string trim(const std::string& str, const std::string& whitespace = " \t");
        std::string reduce(const std::string& str,
            const std::string& fill = " ",
            const std::string& whitespace = " \t");
    };

} //End of namespace

#endif // End of _CONFIG_READER_H_
