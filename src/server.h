#ifndef _SERVER_H_
#define _SERVER_H_

#include "proxy_common.h"
#include "session.h"
#include <atomic>

class TcpServer
{
public:
	TcpServer(ba::io_context& io_context, unsigned short port, size_t bufferSizeKB, int maxSessions);
private:
	void start_accept();
	void handle_accept(TcpSession::pointer new_connection, const boost::system::error_code& error);

	friend class TcpSession;

	ba::io_context& io_context_;
	ba::ip::tcp::acceptor acceptor_;
	ba::thread_pool _pool;
	const size_t bufferSizeKB_;
	std::atomic_int _sessions;
	const int _maxSessions;
};

#endif // _SERVER_H_
