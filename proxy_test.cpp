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

const unsigned short bind_port = 10000;
const std::chrono::milliseconds sleep_time(20000);

int main()
{
	WININIT();
	ba::io_context context;
	std::thread thrContext = std::thread([&](){ context.run(); });
	ba::io_context::work idlework(context);
	try
	{
		tcp_server server(context);
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(sleep_time);
	}
	catch (std::exception &er)
	{
		std::cerr << er.what() << std::endl;
	}

	try {
		context.stop();
		if (thrContext.joinable())
			thrContext.join();
	}
	catch (std::exception& er)
	{
		std::cerr << er.what() << std::endl;
	}

	/*
		попытался разобраться но пока четно
		несколько версий
		1. оно не может закрыть поток и вызывает аборт

		2. чаще всего это происходит если ты хочешь обновить страничку или вернуться на страничку на которой уже бывал
		я не очень уверен но может нужен какой-то механизм разрыва соеденения

		после приблезительно сотни тестов сайты кроме главной странички вики перестали открываться(с ходом она тоже перестала открывать)
	*/
	return 0;
}
