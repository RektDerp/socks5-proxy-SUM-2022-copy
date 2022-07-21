#include "server.h"
#include <thread>

tcp_server::tcp_server(ba::io_context& io_context, unsigned short port, size_t bufferSizeKB,
	int maxSessions): 
	io_context_(io_context),
	acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)),
	bufferSizeKB_(bufferSizeKB),
	_pool(std::thread::hardware_concurrency() * 2),
	_sessions(0),
	_maxSessions(maxSessions)
{
	log("trace") << "[server] =============================================\n"
		<< "[server] Staring proxy server with given parameters:\n"
		<< "[server] port: " << port << "\n"
		<< "[server] buffer size (per connection): " << bufferSizeKB << " KB" << "\n"
		<< "[server] max sessions: " << maxSessions
		<< "\n[server] =============================================\n";
	start_accept();
}

void tcp_server::start_accept()
{
	while (_maxSessions > 0 && _sessions == _maxSessions) {
		std::this_thread::yield();
	}
	
	log("trace") << "[server] Current connections: " << _sessions << "\n"
			<< "[server] waiting for new client... " << acceptor_.local_endpoint() << "\n";
	
	session::pointer new_connection = session::create(this, io_context_, bufferSizeKB_);
	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(session::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		log("trace") << "[server] client connected:\n";
		++_sessions;
		ba::post(_pool, [new_connection] { new_connection->start(); });
	}
	start_accept();
}