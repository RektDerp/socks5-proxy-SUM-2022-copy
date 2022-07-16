#include "server.h"
#include "LogConfigReader.h"
#include <stdexcept>
#ifdef STAT
#include "stat_db_service.h"
#endif
#include <thread>

using namespace cppsecrets;
#ifdef _WIN32
    #include <windows.h>
    #define WININIT() {\
	    SetConsoleCP(1251);\
	    SetConsoleOutputCP(1251);\
	}
const char defaultConfigPath [] = "config.txt";
#else
    #define WININIT() {}
const char defaultConfigPath [] = "/etc/socks5-config.txt";
#endif

int main(int argc, char **argv)
{
	try{
#ifdef STAT
	using namespace proxy::stat;
#ifdef __linux__
	db_service::getInstance("/tmp/sessions_stat.db");
#else 
	db_service::getInstance(); // this initializes table
#endif // __linux__
#endif // STAT
	}
	catch(std::runtime_error &e){
		std::cerr << "Exception in db_service::getInstance(): " << e.what() << std::endl;
	}
	WININIT();
	int port = 1080;
	int bufferSizeKB = 100;
	int maxSessions = 10000;
	if(argc < 2){
		std::cout << "No config file specified, using default: " << defaultConfigPath << std::endl;
		LogConfigReader::configFilePath = defaultConfigPath;
	}
	else{
		LogConfigReader::configFilePath = argv[1];
	}
	LogConfigReader* config = LogConfigReader::getInstance();
	config->getValue("listen_port", port);
	config->getValue("buffer_size_kb", bufferSizeKB);
	config->getValue("max_sessions", maxSessions);

	ba::io_context context;
	tcp_server tcp_server(context, port, bufferSizeKB, maxSessions);

	try
	{
		context.run();
	}
	catch (std::exception& er)
	{
		{
			std::ostringstream tmp;
			tmp << "[main] " << er.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << "[main] " << er.what() << std::endl;
	}
	return 0;
}
