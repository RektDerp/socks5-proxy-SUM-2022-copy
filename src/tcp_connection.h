#ifndef _TCP_CONNECTION_H
#define _TCP_CONNECTION_H

#include "proxy_common.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>

struct ver_packet
{
	char ver;
	char nMethod;
	char Method[255];
};

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(ba::io_context& io_context)
	{
		return pointer(new tcp_connection(io_context));
	}

	ba::ip::tcp::socket& socket()
	{
		return client_socket_;
	}

	void start()
	{
		init();

		client_read();
		server_read();
	}

private:
	tcp_connection(ba::io_context& io_context)
		: client_socket_(io_context), server_socket_(io_context)
	{
	}

	void init();
	void client_read();
	void server_read();
	void client_read_handle(const bs::error_code& error, size_t bytes_transferred);
	void server_read_handle(const bs::error_code& error, size_t bytes_transferred);

	ba::ip::tcp::socket client_socket_;
	ba::ip::tcp::socket server_socket_;
	boost::array<unsigned char, BUFFER_LEN> client_buf_ {};
	boost::array<unsigned char, BUFFER_LEN> server_buf_ {};
};

#endif // _TCP_CONNECTION_H