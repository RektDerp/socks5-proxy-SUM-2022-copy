#include "tcp_connection.h"

bool tcp_connection::init()
{
	std::cout << "[connection] started negotiations with " << client_socket_.remote_endpoint() << std::endl;
	{
		std::ostringstream tmp;
		tmp << "[connection] started negotiations with " << client_socket_.remote_endpoint() << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	bs::error_code ec;

	size_t n = client_socket_.read_some(ba::buffer(client_buf_), ec);

	if (ec)
	{
		{
			std::ostringstream tmp;
			tmp << ec.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << ec.what() << std::endl;
		return false;
	}

	ver_packet packet;
	packet.ver = client_buf_[0];
	packet.nMethod = client_buf_[1];

	client_buf_[0] = packet.ver;
	client_buf_[1] = 0;
	ba::write(client_socket_, ba::buffer(client_buf_, 2), ec);

	if (ec)
	{
		{
			std::ostringstream tmp;
			tmp << ec.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << ec.what() << std::endl;
		return false;
	}

	/*+----+-----+-------+------+----------+----------+
		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+*/

	n = client_socket_.read_some(ba::buffer(client_buf_), ec);

	if (ec)
	{
		{
			std::ostringstream tmp;
			tmp << ec.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << ec.what() << std::endl;
		return false;
	}

	std::string raw_ip_address = formIpAddressString(client_buf_, 4);
	unsigned short server_port = ((int) client_buf_[8] << 8) | (int) client_buf_[9];
	ba::ip::address ip_address = ba::ip::address::from_string(raw_ip_address, ec);

	if (ec)
	{
		std::cout
			<< "[connection] Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: " << ec.message();
		{
			std::ostringstream tmp;
			tmp
				<< "[connection] Failed to parse the IP address. Error code = "
				<< ec.value() << ". Message: " << ec.message();
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
		return false;
	}

	ba::ip::tcp::endpoint ep(ip_address, server_port);
	std::cout << "[connection] connecting to destination server..."
			  << " at address : " << ep << std::endl;
	{
		std::ostringstream tmp;
		tmp << "[connection] connecting to destination server..."
			<< " at address : " << ep << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	server_socket_.connect(ep, ec); // todo: add timeout ?
	if (ec) {
		{
			std::ostringstream tmp;
			tmp << "[connection] error occured during connection to destination address."
				<< std::endl << ec.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << "[connection] error occured during connection to destination address."
			<< std::endl << ec.what() << std::endl;
		return false;
	}

	bind_port_ = server_socket_.local_endpoint().port();
	std::cout << "[connection] server connected on local port " << bind_port_ << std::endl;
	{
		std::ostringstream tmp;
		tmp << "[connection] server connected on local port " << bind_port_ << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
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
	client_buf_[8] = (bind_port_ & 0xFF00) >> 8;
	client_buf_[9] = bind_port_ & 0x00FF;

	ba::write(client_socket_, ba::buffer(client_buf_, 10), ec);
	if (ec)
	{
		{
			std::ostringstream tmp;
			tmp << ec.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << ec.what() << std::endl;
		return false;
	}

	return true;
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
			<< bind_port_
			<< "] " <<  " Stopped reading - client socket is closed."
			<< std::endl;
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << " Stopped reading - client socket is closed."
				<< std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
	}
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
			<< bind_port_
			<< "] Stopped reading - server socket is closed."
			<< std::endl;
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] Stopped reading - server socket is closed."
				<< std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
	}
}

void tcp_connection::client_read_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		std::cout << "[" << bind_port_ << "] Client EOF." << std::endl;
		return;
	}
	else if (error.value() > 0)
	{
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << "error occured while reading client: "
				<< error.what() << std::endl;;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << "["
			<< bind_port_
			<< "] " << "error occured while reading client: "
			      << error.what() << std::endl;
		return;
	} 
	
	if (bytes_transferred > 0)
	{
		if (writeToSocket(server_socket_, client_buf_, bytes_transferred, true))
		{
			client_read();
		}
		else {
			close();
		}
	}
	else {
		std::cout << bind_port_
			<< "no bytes transferred, closing connection...\n";
	}
}

void tcp_connection::server_read_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		std::cout << "[" << bind_port_ << "] Server EOF." << std::endl;
		return;
	}
	else if (error.value() > 0)
	{
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << "error occured while reading server: "
				<< error.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << "["
			<< bind_port_
			<< "] " << "error occured while reading server: "
			<< error.what() << std::endl;
		return;
	}

	if (bytes_transferred > 0)
	{
		if (writeToSocket(client_socket_, server_buf_, bytes_transferred, false))
		{
			server_read();
		}
		else
		{
			close();
		}
	}
	else {
		std::cout << "[" << bind_port_
			<< "] no bytes transferred...\n";
	}
}

bool tcp_connection::writeToSocket(ba::ip::tcp::socket& socket, barray buffer, size_t len, bool isServer)
{
	std::string target = isServer ? "server" : "client"; // todo: optimise
	std::cout << "["
		<< bind_port_
		<< "] " << "Sending " << len << " bytes to " << target << std::endl;
	{
		std::ostringstream tmp;
		tmp << "["
			<< bind_port_
			<< "] " << "Sending " << len << " bytes to " << target << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	bs::error_code ec;
	ba::write(socket, ba::buffer(buffer, len), ec);
	if (ec) {
		std::cout << "["
			<< bind_port_
			<< "] Error occured while writing: " << ec.what() << std::endl;
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << ec.what() << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
		return false;
	}
	
	std::cout << "["
		<< bind_port_
		<< "] Complete!\n";
	{
		std::ostringstream tmp;
		tmp << "["
			<< bind_port_
			<< "] Complete!\n";
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	
	return true;
}

void tcp_connection::close()
{
	std::cout << "[" << bind_port_ << "] Closing sockets..." << std::endl;
	try {
		client_socket_.close();
		server_socket_.close();
	}
	catch (const bs::system_error& e)
	{
		std::cout << "[" << bind_port_ << "] Error occurred during closing: " << e.what() << std::endl;
	}
}