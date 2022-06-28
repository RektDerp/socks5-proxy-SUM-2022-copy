#include "tcp_connection.h"

void tcp_connection::init()
{
	std::cout << "started negotiations on " << client_socket_.local_endpoint() << std::endl;
	bs::error_code error;

	size_t n = client_socket_.read_some(ba::buffer(client_buf_));
	std::cout << "read " << n << " bytes\n";
	ver_packet packet;
	packet.ver = client_buf_[0];
	packet.nMethod = client_buf_[1];

	std::cout << (int)packet.ver << "  " << (int)packet.nMethod << "\n";

	for (int i = 0; i < packet.nMethod; i++)
	{
		packet.Method[i] = client_buf_[2 + i];
		std::cout << (int)packet.Method[i] << std::endl;
	}

	client_buf_[0] = packet.ver;
	client_buf_[1] = 0;
	ba::write(client_socket_, ba::buffer(client_buf_, 2));

	/*+----+-----+-------+------+----------+----------+
		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+*/

	n = client_socket_.read_some(ba::buffer(client_buf_));
	std::cout << "read " << n << " bytes\n";
	for (int i = 0; i < 10; i++)
	{
		std::cout << (int)client_buf_[i] << std::endl;
	}

	std::string raw_ip_address =
		std::to_string((int)client_buf_[4]) +
		'.' +
		std::to_string((int)client_buf_[5]) +
		'.' +
		std::to_string((int)client_buf_[6]) +
		'.' +
		std::to_string((int)client_buf_[7]);

	unsigned short server_port = ((int)client_buf_[8] << 8) | (int)client_buf_[9];
	std::cout << "dst address: " << raw_ip_address << ':' << server_port << std::endl;

	std::cout << "connecting to server..." << std::endl;

	boost::system::error_code ec;
	ba::ip::address ip_address = ba::ip::address::from_string(raw_ip_address, ec);

	if (ec.value() != 0)
	{
		std::cout
			<< "Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: " << ec.message();
		throw ec;
	}

	ba::ip::tcp::endpoint ep(ip_address, server_port);
	std::cout << "at address: " << ep << std::endl;
	server_socket_.connect(ep);
	std::cout << "server connected." << std::endl;

	/*+----+-----+-------+------+----------+----------+
		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+*/

		// client_buf_[0] = packet.ver;
	client_buf_[1] = 0;
	client_buf_[4] = 127;
	client_buf_[5] = 0;
	client_buf_[6] = 0;
	client_buf_[7] = 1;
	client_buf_[8] = (listen_port & 0xFF00) >> 8;
	client_buf_[9] = listen_port & 0x00FF;

	for (int i = 0; i < 10; i++)
	{
		std::cout << (unsigned short)client_buf_[i] << std::endl;
	}

	ba::write(client_socket_, ba::buffer(client_buf_, 10));
}

void tcp_connection::client_read()
{
	client_socket_.async_read_some(ba::buffer(client_buf_),
		boost::bind(&tcp_connection::client_read_handle, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::client_read_handle(const bs::error_code& error, size_t bytes_transferred)
{
	std::cout << "handling client...\n";
	if (error)
	{
		std::cout << error.what() << std::endl;
		return;
	}

	if (bytes_transferred > 0)
	{
		std::cout << "Sending " << bytes_transferred << " bytes to client\n";
		ba::write(server_socket_, ba::buffer(client_buf_, bytes_transferred));
		std::cout << "complete!\n";
	}
	client_read();
}

void tcp_connection::server_read()
{
	server_socket_.async_read_some(ba::buffer(server_buf_),
		boost::bind(&tcp_connection::server_read_handle, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::server_read_handle(const bs::error_code& error, size_t bytes_transferred)
{
	std::cout << "handling server...\n";
	if (error)
	{
		std::cout << error.what() << std::endl;
		return;
	}

	if (bytes_transferred > 0)
	{
		std::cout << "Sending " << bytes_transferred << " bytes to client\n";
		ba::write(client_socket_, ba::buffer(server_buf_, bytes_transferred));
		std::cout << "complete!\n";
	}
	server_read();
}