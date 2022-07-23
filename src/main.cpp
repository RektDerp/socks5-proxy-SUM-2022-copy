#include "server.h"
#include "LogConfigReader.h"
#include "Logger.h"
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
const char defaultDatabasePath [] = R"(./sessions_stat.db)";
#else
    #define WININIT() {}
const char defaultConfigPath [] = "/etc/socks5-config.txt";
#endif

void initDb();

int main(int argc, char **argv)
{
	WININIT();
	LogConfigReader::configFilePath = defaultConfigPath;
	std::cout << "Config file: " << LogConfigReader::configFilePath << std::endl;
	LogConfigReader* config = LogConfigReader::getInstance();

	initDb();
	// default server parameters
	int port = 1080;
	int bufferSizeKB = 100;
	int maxSessions = 0;
	
	config->dumpFileValues();
	config->dumpUsersValues();
	config->getValue("listen_port", port);
	config->getValue("buffer_size_kb", bufferSizeKB);
	config->getValue("max_sessions", maxSessions);
	ba::io_context context;

	try {
		tcp_server tcp_server(context, port, bufferSizeKB, maxSessions);
	}
	catch (const std::exception& ex) {
		log(ERROR_LOG) << "Error occurred while setting up server: \n" << ex.what();
		return -1;
	}

	try {
		context.run();
	}
	catch (const std::exception& ex) {
		log(ERROR_LOG) << "Error in io_context: " << ex.what();
		return -1;
	}


	/*std::thread thread([&] {
		);

	thread.join();*/

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
