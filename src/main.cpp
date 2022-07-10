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
#else
    #define WININIT() {}
#endif

int main()
{
	WININIT();
	int port = 0;
	int bufferSizeKB = 0;
	LogConfigReader* config = LogConfigReader::getInstance();
	config->getValue("listen_port", port);
	config->getValue("buffer_size_kb", bufferSizeKB);
#ifdef STAT
	using namespace proxy::stat;
	createDB();
	createTable();
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



		std::cout << "============================================================\n";
		for (const Session& s : selectAll()) {
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
