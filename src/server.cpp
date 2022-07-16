#include "server.h"
#include <thread>

tcp_server::tcp_server(ba::io_context& io_context, unsigned short port, size_t bufferSizeKB,
	int maxSessions): 
	io_context_(io_context),
	acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)),
	bufferSizeKB_(bufferSizeKB),
	_pool(100),
	_sessions(0),
	_maxSessions(maxSessions)
{
	std::cout << "[server] =============================================\n"
		<< "[server] Staring proxy server with given parameters:\n"
		<< "[server] port: " << port << std::endl
		<< "[server] buffer size (per connection): " << bufferSizeKB << " KB" << std::endl
		<< "[server] max sessions: " << maxSessions
		<< "\n[server] =============================================\n";
	start_accept();
}

void tcp_server::start_accept()
{
	while (_sessions == _maxSessions) {
		std::this_thread::yield();
	}

	std::cout << "[server] Current connections: " << _sessions << std::endl;
	std::cout << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
	session::pointer new_connection = session::create(this, io_context_, bufferSizeKB_);
	/*{
		std::ostringstream tmp;
		tmp << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}*/
	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(session::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		//std::cout << "[server] client connected" << std::endl;
		/*{
			std::ostringstream tmp;
			tmp << "[server] client connected" << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
		++_sessions;
		ba::post(_pool, [new_connection]() { new_connection->start(); });
	}
	start_accept();
}