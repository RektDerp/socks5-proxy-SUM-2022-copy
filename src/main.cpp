#include "server.h"
#include "ConfigReader.h"
#include <stdexcept>
#ifdef STAT
#include "stat_db_service.h"
#endif
#include <thread>
#include <memory>

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
	ConfigReader::configFilePath = defaultConfigPath;
	std::cout << "Config path: " << ConfigReader::configFilePath << std::endl;
	ConfigReader* config = ConfigReader::getInstance();
	initDb();

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

void initDb()
{
#ifdef STAT
	using namespace proxy::stat;
#ifdef __linux__
	DatabaseService::getInstance("/tmp/sessions_stat.db");
#else 
	DatabaseService::getInstance(); // this initializes table
#endif // __linux__
#endif // STAT
}
