#ifndef _SERVER_H_
#define _SERVER_H_

#include "proxy_common.h"
#include "tcp_connection.h"

class tcp_server
{
public:
	tcp_server(ba::io_context& io_context);
private:
	void start_accept();
	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error);

	ba::io_context& io_context_;
	ba::ip::tcp::acceptor acceptor_;

};

#endif // _SERVER_H_
