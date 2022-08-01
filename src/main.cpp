#include "server.h"
#include "socks5.h"
#include "ConfigReader.h"
#include <stdexcept>
#include "stat_db_service.h"
#include <thread>
#include <memory>
#define PATHSIZE 256
extern char logFileName [];
#ifdef _WIN32
    #include <windows.h>
    char defaultConfigPath [PATHSIZE] = ".\\config.txt";
    char defaultDatabasePath [PATHSIZE] = ".\\sessions_stat.db";
	char pwd[PATHSIZE];
    #define WININIT() {\
	    SetConsoleCP(1251);\
	    SetConsoleOutputCP(1251);\
	    GetModuleFileName(NULL, pwd, PATHSIZE);\
	    *(strrchr(pwd, '\\') + 1) = '\0';\
	    strcpy(defaultConfigPath, pwd);\
	    strcpy(defaultDatabasePath, pwd);\
	    strcpy(logFileName, pwd);\
	    strcat(defaultDatabasePath, "sessions_stat.db");\
	    strcat(defaultConfigPath, "config.txt");\
	    strcat(logFileName, "MyLogFile.log");\
	}
#else
	#define WININIT() {}
	const char defaultConfigPath [PATHSIZE] = "/etc/socks5-config.txt";
	const char defaultDatabasePath[PATHSIZE] = "/tmp/sessions_stat.db";
	strcpy(logFileName, "/tmp/MyLogFile.log");
#endif

int main(int argc, char** argv)
{
	using namespace proxy;
	WININIT();
	ConfigReader::configFilePath = defaultConfigPath;
	std::cout << "Config path: " << ConfigReader::configFilePath << std::endl;
	std::cout << "Database path: " << defaultDatabasePath << std::endl;
	ConfigReader& config = ConfigReader::getInstance();
	config.dumpFileValues();
	try {
		// this initializes the table
		proxy::DatabaseService::getInstance(defaultDatabasePath);
	}

	catch (const DatabaseException& ex) {
		log(FATAL_LOG) << "Database was not created: " << ex.what();
		return -1;
	}

	// default server parameters
	int port = 1080;
	int bufferSizeKB = 100;
	int maxSessions = 0;
	config.getValue("listen_port", port);
	config.getValue("buffer_size_kb", bufferSizeKB);
	config.getValue("max_sessions", maxSessions);
	config.getValue("auth", Socks5::AUTH_FLAG);

	ba::io_context context;

	std::unique_ptr<TcpServer> server = nullptr;
	try {
		server = std::make_unique<TcpServer>(context, port, bufferSizeKB, maxSessions);
	}
	catch (const std::exception& ex) {
		log(FATAL_LOG) << "Failed to start server: " << ex.what();
		return -1;
	}

	std::thread thread([&] { 
		try {
			context.run();
		}
		catch (const std::exception& ex) {
			log(FATAL_LOG) << "io_context throwed exception: " << ex.what();
		}
	});
	thread.join();
	log(INFO_LOG) << "Stopped server.";
	return 0;
}
