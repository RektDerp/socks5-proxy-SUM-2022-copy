#include "server.h"
#include "socks5.h"
#include "ConfigReader.h"
#include <stdexcept>
#include "stat_db_service.h"
#include <thread>
#include <memory>
#include <string.h>
#define PATHSIZE 256
#ifdef _WIN32
    #ifndef UNICODE
    #define UNICODE
    #endif

    #ifndef _UNICODE
    #define _UNICODE
    #endif

    #include <windows.h>
    #include <stringapiset.h>
	char defaultLogPath[PATHSIZE] = ".\\server.txt";
    char defaultConfigPath [PATHSIZE] = ".\\config.txt";
    char defaultDatabasePath [PATHSIZE] = ".\\sessions_stat.db";
	wchar_t pwd[PATHSIZE];
	char utf8_pwd[PATHSIZE];

	void init()
	{
		SetConsoleCP(1251); 
		SetConsoleOutputCP(1251); 
		GetModuleFileNameW(NULL, pwd, PATHSIZE); 
		* (wcsrchr(pwd, '\\') + 1) = '\0'; 
		WideCharToMultiByte(CP_UTF8, 0, pwd, -1, utf8_pwd, PATHSIZE, NULL, NULL); 
		strcpy(defaultDatabasePath, utf8_pwd); 
		strcat(defaultDatabasePath, "sessions_stat.db"); 
	
		GetModuleFileNameA(NULL, utf8_pwd, PATHSIZE); 
		* (strrchr(utf8_pwd, '\\') + 1) = '\0'; 
		strcpy(defaultConfigPath, utf8_pwd); 
		strcat(defaultConfigPath, "config.txt"); 
		strcpy(defaultLogPath, utf8_pwd);
		strcat(defaultLogPath, "server.log");
	}
#else
	char defaultLogPath[PATHSIZE] = "/tmp/server.log";
	const char defaultConfigPath [PATHSIZE] = "/etc/socks5-config.txt";
	const char defaultDatabasePath[PATHSIZE] = "/tmp/sessions_stat.db";

	void init(){}
#endif

int main(int argc, char** argv)
{
	using namespace proxy;
	try {
		init();
		Logger::logFileName = defaultLogPath;
		ConfigReader::configFilePath = defaultConfigPath;
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
	}
	catch (const std::runtime_error& er) {
		log(FATAL_LOG) << er.what();
	}
	
	log(INFO_LOG) << "Stopped server.";
	return 0;
}
