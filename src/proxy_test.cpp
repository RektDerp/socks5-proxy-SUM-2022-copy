#include "server.h"

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
	
	try
	{
		ba::io_context context;
		tcp_server server(context);
		context.run();
	}
	catch (std::exception &er)
	{
		std::cerr << "[main] " << er.what() << std::endl;
	}

	return 0;
}
