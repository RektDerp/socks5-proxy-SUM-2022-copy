#ifndef _SESSION_H_
#define _SESSION_H_

#include "proxy_common.h"
#include "Logger.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
namespace proxy {
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
			return _client_socket;
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
		std::unique_ptr<Socks> _socks;
		ba::io_context& _io_context;

		ba::ip::tcp::socket _client_socket;
		ba::ip::tcp::socket _server_socket;

		unsigned short _bind_port = 0;

		bvec _client_buf;
		bvec _server_buf;

		std::atomic_bool _isClosed = false;
	};
} // namespace proxy
#endif // _SESSION_H_
