#include "server.h"
#include "LogConfigReader.h"
#include <stdexcept>
#ifdef STAT
#include "stat_db_service.h"
#endif
#include <thread>

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

void initDb();

int main(int argc, char **argv)
{
	WININIT();
	initDb();
	// default server parameters
	int port = 1080;
	int bufferSizeKB = 100;
	int maxSessions = 0;
	if(argc < 2) {
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
	
	std::thread thread([&] {
		context.run();
		});
	thread.join();

	return 0;
}

void initDb()
{
#ifdef STAT
	using namespace proxy::stat;
#ifdef __linux__
	db_service::getInstance("/tmp/sessions_stat.db");
#else 
	db_service::getInstance(); // this initializes table
#endif // __linux__
#endif // STAT
}
