#include "server.h"
#include "ConfigReader.h"
#include <stdexcept>
#include "stat_db_service.h"
#include <thread>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
    #define WININIT() {\
	    SetConsoleCP(1251);\
	    SetConsoleOutputCP(1251);\
	}
	const char defaultConfigPath [] = "config.txt";
	const char defaultDatabasePath [] = "./sessions_stat.db";
#else
	#define WININIT() {}
	const char defaultConfigPath [] = "/etc/socks5-config.txt";
	const char defaultDatabasePath[] = "/tmp/sessions_stat.db";
#endif

int main(int argc, char** argv)
{
	using namespace proxy;
	WININIT();
	ConfigReader::configFilePath = defaultConfigPath;
	std::cout << "Config path: " << ConfigReader::configFilePath << std::endl;
	std::cout << "Database path: " << defaultDatabasePath << std::endl;
	ConfigReader* config = ConfigReader::getInstance();
	try {
		// this initializes the table
		proxy::DatabaseService::getInstance(defaultDatabasePath);
	}
	catch (const DatabaseException& ex) {
		log(ERROR_LOG) << "Database was not created: " << ex.what();
		return -1;
	}

	// default server parameters
	int port = 1080;
	int bufferSizeKB = 100;
	int maxSessions = 0;
	config->getValue("listen_port", port);
	config->getValue("buffer_size_kb", bufferSizeKB);
	config->getValue("max_sessions", maxSessions);

	ba::io_context context;

	std::unique_ptr<TcpServer> server = nullptr;
	try {
		server = std::make_unique<TcpServer>(context, port, bufferSizeKB, maxSessions);
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
