#include "server.h"
#include <thread>

TcpServer::TcpServer(ba::io_context& io_context, unsigned short port, size_t bufferSizeKB,
	size_t maxSessions): 
	_io_context(io_context),
	_acceptor(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)),
	_bufferSizeKB(bufferSizeKB),
	_pool(std::thread::hardware_concurrency() * 2),
	_sessions(0),
	_maxSessions(maxSessions)
{
	log(TRACE_LOG) << "[server] =============================================";
	log(TRACE_LOG) << "[server] Starting proxy server with given parameters:";
	log(TRACE_LOG) << "[server] port: " << port;
	log(TRACE_LOG) << "[server] buffer size (per connection): " << bufferSizeKB << " KB";
	log(TRACE_LOG) << "[server] max sessions: " << maxSessions;
	log(TRACE_LOG) << "[server] =============================================";
	start_accept();
}

void TcpServer::start_accept()
{
	while (_maxSessions > 0 && _sessions == _maxSessions) {
		std::this_thread::yield();
	}
	
	log(TRACE_LOG) << "[server] Current connections: " << _sessions;
	log(TRACE_LOG) << "[server] waiting for new client... " << _acceptor.local_endpoint();
	
	TcpSession::pointer new_connection = TcpSession::create(this, _io_context, _bufferSizeKB);
	_acceptor.async_accept(new_connection->socket(),
		boost::bind(&TcpServer::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void TcpServer::handle_accept(TcpSession::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		log(TRACE_LOG) << "[server] Client connected";
		++_sessions;
		ba::post(_pool, [new_connection] { new_connection->start(); });
	}
	start_accept();
}