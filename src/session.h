#ifndef _SESSION_H
#define _SESSION_H

#include "proxy_common.h"
#include "Logger.h"
#include "statWriter.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>

const int BUFFER_LEN = 16 * 1024;

using barray = boost::array<unsigned char, BUFFER_LEN>;
class socks5_impl;
class session : public boost::enable_shared_from_this<session>
{
public:
	typedef boost::shared_ptr<session> pointer;
	static session::pointer create(ba::io_context& io_context)
	{	
		return boost::make_shared<session>(io_context);
	}
	session(ba::io_context& io_context);
	~session();

	ba::ip::tcp::socket& socket()
	{
		return client_socket_;
	}

	void start();
	
	unsigned char readByte(bs::error_code& ec);
	void readBytes(bvec& bytes, bs::error_code& ec);
	void writeBytes(const bvec& bytes, bs::error_code& ec);

	unsigned short connect(ba::ip::tcp::resolver::query& query, bs::error_code& ec);

private:
	void client_read();
	void server_read();
	void close();
	bool writeToSocket(ba::ip::tcp::socket& socket, barray buffer, size_t len, bool isServer);
	void client_handle(const bs::error_code& error, size_t bytes_transferred);
	void server_handle(const bs::error_code& error, size_t bytes_transferred);

	ba::io_context& io_context_;

	ba::ip::tcp::socket client_socket_;
	ba::ip::tcp::socket server_socket_;

	unsigned short      bind_port_ = 0;

	barray client_buf_{};
	barray server_buf_{};

	socks5_impl* impl;

	std::stringstream logString;
};

#endif // _SESSION_H