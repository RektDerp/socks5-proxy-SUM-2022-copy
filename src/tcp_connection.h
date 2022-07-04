#ifndef _TCP_CONNECTION_H
#define _TCP_CONNECTION_H

#include "proxy_common.h"
#include "Logger.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <boost/numeric/ublas/vector.hpp>

struct ver_packet
{
	char ver;
	char nMethod;
	char Method[255];
};

const int BUFFER_LEN = 16 * 1024;

using barray = boost::array<unsigned char, BUFFER_LEN>;

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
		if (init()) {
			std::cout << "[connection] Starting session...\n";
			client_read();
			server_read();
		}
		else {
			close();
		}
	}

private:
	tcp_connection(ba::io_context& io_context)
		: client_socket_(io_context), server_socket_(io_context)
	{}

	bool init();
	void client_read();
	void server_read();
	void client_read_handle(const bs::error_code& error, size_t bytes_transferred);
	void server_read_handle(const bs::error_code& error, size_t bytes_transferred);
	void close();
	bool writeToSocket(ba::ip::tcp::socket& socket, barray buffer, size_t len, bool isServer);

	ba::ip::tcp::socket client_socket_;
	ba::ip::tcp::socket server_socket_;

	unsigned short      bind_port_ = 0;

	barray client_buf_{};
	barray server_buf_{};

	std::stringstream logString;
};

inline std::string formIpAddressString(barray& buf, size_t offset)
{
	std::string ip_address;
	for (int i = 0; i < 4; i++)
	{
		ip_address += std::to_string((int)buf[offset + i]);
		if (i != 3) ip_address += '.';
	}
	return ip_address;
}

#endif // _TCP_CONNECTION_H