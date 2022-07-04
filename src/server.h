#ifndef _SERVER_H_
#define _SERVER_H_

#include "proxy_common.h"
#include "session.h"

class tcp_server
{
public:
	tcp_server(ba::io_context& io_context, unsigned short port);
private:
	void start_accept();
	void handle_accept(session::pointer new_connection, const boost::system::error_code& error);

	ba::io_context& io_context_;
	ba::ip::tcp::acceptor acceptor_;
};

#endif // _SERVER_H_
