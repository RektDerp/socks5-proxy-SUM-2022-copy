#include "server.h"
#include "LogConfigReader.h"
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
	LogConfigReader* config = LogConfigReader::getInstance();
	config->getValue("listen_port", port);
	std::cout << port;
	try
	{
		ba::io_context context;
		tcp_server server(context, port);
		context.run();
	}
	catch (std::exception &er)
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
