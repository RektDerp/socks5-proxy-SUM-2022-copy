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
const char defaultDatabasePath [] = R"(./sessions_stat.db)";
#else
    #define WININIT() {}
const char defaultConfigPath [] = "/etc/socks5-config.txt";
#endif

void initDb();

int main(int argc, char** argv)
{
	WININIT();
	LogConfigReader::configFilePath = defaultConfigPath;
	std::cout << "Config path: " << LogConfigReader::configFilePath << std::endl;
	LogConfigReader* config = LogConfigReader::getInstance();
	config->dumpFileValues();
	config->dumpUsersValues();
	initDb();

	// default server parameters
	int port = 1080;
	int bufferSizeKB = 100;
	int maxSessions = 0;
	config->getValue("listen_port", port);
	config->getValue("buffer_size_kb", bufferSizeKB);
	config->getValue("max_sessions", maxSessions);

	ba::io_context context;

	std::unique_ptr<tcp_server> server = nullptr;
	try {
		server = std::make_unique<tcp_server>(context, port, bufferSizeKB, maxSessions);
	}
	catch (const std::exception& ex) {
		log(ERROR_LOG) << "Failed to start server: " << ex.what();
		return -1;
	}

	std::thread thread([&] { 
		try {
			context.run();
		}
		catch (const std::exception& ex) {
			log(ERROR_LOG) << "io_context throwed exception: " << ex.what();
		}
	});
	thread.join();
	log(TRACE_LOG) << "Stopped server.";
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
