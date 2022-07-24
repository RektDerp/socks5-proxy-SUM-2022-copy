#ifndef _SESSION_H_
#define _SESSION_H_

#include "proxy_common.h"
#include "Logger.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>

class Socks;
class TcpServer;

class TcpSession : public boost::enable_shared_from_this<TcpSession>
{
public:
	typedef boost::shared_ptr<TcpSession> pointer;
	static TcpSession::pointer create(TcpServer* server, ba::io_context& io_context, size_t bufferSizeKB)
	{	
		return pointer(new TcpSession(server, io_context, bufferSizeKB));
	}
	
	~TcpSession();

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
	TcpSession(TcpServer* server, ba::io_context& io_context, size_t bufferSizeKB);

	bool createProxy();

	void client_read();
	void server_read();
	
	bool writeToSocket(ba::ip::tcp::socket& socket, bvec& buffer, size_t len, bool isServer);
	void client_handle(const bs::error_code& error, size_t bytes_transferred);
	void server_handle(const bs::error_code& error, size_t bytes_transferred);

	TcpServer* _server;
	ba::io_context& io_context_;

	ba::ip::tcp::socket client_socket_;
	ba::ip::tcp::socket server_socket_;

	unsigned short bind_port_ = 0;

	bvec client_buf_;
	bvec server_buf_;

	Socks* impl_;

	std::stringstream logString;
};

#endif // _SESSION_H_
