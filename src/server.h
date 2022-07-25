#ifndef _SERVER_H_
#define _SERVER_H_

#include "proxy_common.h"
#include "session.h"
#include <atomic>

class TcpServer
{
public:
	TcpServer(ba::io_context& io_context, unsigned short port, size_t bufferSizeKB, size_t maxSessions);
private:
	void start_accept();
	void handle_accept(TcpSession::pointer new_connection, const boost::system::error_code& error);

	friend class TcpSession;

	ba::io_context& _io_context;
	ba::ip::tcp::acceptor _acceptor;
	ba::thread_pool _pool;
	const size_t _bufferSizeKB;
	std::atomic_int _sessions;
	const size_t _maxSessions;
};

#endif // _SERVER_H_
