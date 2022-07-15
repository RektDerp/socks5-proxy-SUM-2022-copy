#include "server.h"
#include "LogConfigReader.h"
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
	#ifdef __linux__
	    proxy::stat::set_db_path("/tmp/sessions_stat.db");
    #endif
	WININIT();
	int port = 0;
	int bufferSizeKB = 0;
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
#ifdef STAT
	using namespace proxy::stat;
	db_service::getInstance(); // this initializes table
#endif
	ba::io_context context;
	tcp_server server(context, port, bufferSizeKB);

	std::thread t1([&]() {
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
	});

#ifdef STAT
	for (;;)
	{
		std::string s;
		std::getline(std::cin, s);

		if(s.compare("quit"))break;

		std::cout << "============================================================\n";
		for (const proxy::stat::session& s : db_service::getInstance().selectAll()) {
			std::cout << s.id << ","
				<< s.user << ","
				<< (s.is_active ? "active" : "inactive") << ","
				<< s.src_addr << ","
				<< s.src_port << ","
				<< s.dst_addr << ","
				<< s.dst_port << ","
				<< s.bytes_sent << ","
				<< s.bytes_recv
				<< std::endl;
		}
	}
#endif // STAT
	t1.join();
	return 0;
}
