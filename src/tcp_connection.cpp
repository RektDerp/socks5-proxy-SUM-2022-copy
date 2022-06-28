#include "tcp_connection.h"

void tcp_connection::init()
{
	std::cout << "[connection] started negotiations with " << client_socket_.remote_endpoint() << std::endl;
	bs::error_code ec;

	size_t n = client_socket_.read_some(ba::buffer(client_buf_), ec);

	if (ec)
	{
		std::cerr << ec.what() << std::endl;
		return;
	}

	ver_packet packet;
	packet.ver = client_buf_[0];
	packet.nMethod = client_buf_[1];

	client_buf_[0] = packet.ver;
	client_buf_[1] = 0;
	ba::write(client_socket_, ba::buffer(client_buf_, 2), ec);

	if (ec)
	{
		std::cerr << ec.what() << std::endl;
		return;
	}

	/*+----+-----+-------+------+----------+----------+
		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+*/

	n = client_socket_.read_some(ba::buffer(client_buf_), ec);

	if (ec)
	{
		std::cerr << ec.what() << std::endl;
		return;
	}

	std::string raw_ip_address = formIpAddressString(client_buf_, 4);
	unsigned short server_port = ((int) client_buf_[8] << 8) | (int) client_buf_[9];
	ba::ip::address ip_address = ba::ip::address::from_string(raw_ip_address, ec);

	if (ec)
	{
		std::cout
			<< "[connection] Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: " << ec.message();
		return;
	}

	ba::ip::tcp::endpoint ep(ip_address, server_port);
	std::cout << "[connection] connecting to destination server..."
			  << " at address : " << ep << std::endl;
	server_socket_.connect(ep, ec);
	if (ec) {
		std::cerr << "[connection] error occured during connection to destination address."
			<< std::endl << ec.what() << std::endl;
		return;
	}

	unsigned short bind_port = server_socket_.local_endpoint().port();
	std::cout << "[connection] server connected on local port " << bind_port << std::endl;

	/*+----+-----+-------+------+----------+----------+
		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+*/

 // client_buf_[0] = packet.ver;
	client_buf_[1] = 0;
	client_buf_[4] = 0;
	client_buf_[5] = 0;
	client_buf_[6] = 0;
	client_buf_[7] = 0;
	client_buf_[8] = (bind_port & 0xFF00) >> 8;
	client_buf_[9] = bind_port & 0x00FF;

	ba::write(client_socket_, ba::buffer(client_buf_, 10), ec);
	if (ec)
	{
		std::cerr << ec.what() << std::endl;
		return;
	}
}

void tcp_connection::client_read()
{
	if (client_socket_.is_open()) {
		client_socket_.async_read_some(ba::buffer(client_buf_),
			boost::bind(&tcp_connection::client_read_handle, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		std::cout << "["
			<< server_socket_.local_endpoint().port()
			<< "] " <<  " - client socket is closed."
			<< std::endl;
	}
}

void tcp_connection::client_read_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() > 0)
	{
		std::cerr << "["
			<< server_socket_.local_endpoint().port()
			<< "] " << "error occured while reading client: "
			      << error.what() << std::endl;
		return;
	}

	if (bytes_transferred > 0)
	{
		std::cout << "["
			<< server_socket_.local_endpoint().port()
			<< "] " << "Sending " << bytes_transferred << " bytes to client\n";
		bs::error_code ec;
		ba::write(server_socket_, ba::buffer(client_buf_, bytes_transferred), ec);
		if (ec) {
			std::cout << "["
				<< server_socket_.local_endpoint().port()
				<< "] " << ec.what() << std::endl;
			return;
		}
		else {
			std::cout << "["
				<< server_socket_.local_endpoint().port()
				<< "] " << "complete!\n";
		}
	}
	client_read();
}

void tcp_connection::server_read()
{
	if (server_socket_.is_open()) {
		server_socket_.async_read_some(ba::buffer(server_buf_),
			boost::bind(&tcp_connection::server_read_handle, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		std::cout << "["
			<< server_socket_.local_endpoint().port()
			<< "] - server socket is closed."
			<< std::endl;
	}
}

void tcp_connection::server_read_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() > 0)
	{
		std::cerr << "["
			<< server_socket_.local_endpoint().port()
			<< "] " << "error occured while reading client: "
				  << error.what() << std::endl;
		return;
	}

	if (bytes_transferred > 0)
	{
		std::cout << "["
			<< server_socket_.local_endpoint().port()
			<< "] " << "Sending " << bytes_transferred << " bytes to client\n";
		bs::error_code ec;
		ba::write(client_socket_, ba::buffer(server_buf_, bytes_transferred), ec);
		if (ec) {
			std::cout << "["
				<< server_socket_.local_endpoint().port()
				<< "] " << ec.what() << std::endl;
			return;
		}
		else {
			std::cout << "["
				<< server_socket_.local_endpoint().port()
				<< "] " << "complete!\n";
		}
	}
	server_read();
}