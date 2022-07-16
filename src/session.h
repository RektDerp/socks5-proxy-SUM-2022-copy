#ifndef _SESSION_H_
#define _SESSION_H_

#include "proxy_common.h"
#include "Logger.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>

#ifdef STAT_CSV
#include "statWriter.h"
#endif

class socks5_impl;
class tcp_server;

class session : public boost::enable_shared_from_this<session>
{
public:
	typedef boost::shared_ptr<session> pointer;
	static session::pointer create(tcp_server* server, ba::io_context& io_context, size_t bufferSizeKB)
	{	
		return pointer(new session(server, io_context, bufferSizeKB));
	}
	
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
	void close();
private:
	session(tcp_server* server, ba::io_context& io_context, size_t bufferSizeKB);

	void client_read();
	void server_read();
	
	bool writeToSocket(ba::ip::tcp::socket& socket, bvec& buffer, size_t len, bool isServer);
	void client_handle(const bs::error_code& error, size_t bytes_transferred);
	void server_handle(const bs::error_code& error, size_t bytes_transferred);

	tcp_server* _server;
	ba::io_context& io_context_;

	ba::ip::tcp::socket client_socket_;
	ba::ip::tcp::socket server_socket_;

	unsigned short bind_port_ = 0;

	bvec client_buf_;
	bvec server_buf_;

	socks5_impl* impl_;

	std::stringstream logString;
};

#endif // _SESSION_H_