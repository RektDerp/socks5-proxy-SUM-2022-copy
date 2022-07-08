#include "session.h"
#include <memory>
#include "socks5_impl.h"

session::session(ba::io_context& io_context)
	: client_socket_(io_context), server_socket_(io_context),
	impl(new socks5_impl(this))
{}

session::~session()
{
	delete impl;
}

void session::start()
{
	if (impl->init()) {
		std::cout << "[session] Starting session...\n";
		client_read();
		server_read();
	}
	else {
		close();
	}
}

// todo add timeout
unsigned char session::readByte(bs::error_code& ec)
{
	boost::asio::read(client_socket_, ba::buffer(client_buf_),
		boost::asio::transfer_exactly(1), ec);
	return client_buf_[0];
}

void session::readBytes(bvec& vec, bs::error_code& ec)
{
	boost::asio::read(client_socket_, ba::buffer(vec), ec);
}

void session::writeBytes(const bvec& bytes, bs::error_code& ec)
{
	ba::write(client_socket_, ba::buffer(bytes), ec);
}

unsigned short session::connect(ba::ip::tcp::endpoint& endpoint, bs::error_code& ec)
{
	std::cout << "[session] connecting to destination server..." << " at address : " << endpoint << std::endl;
	{
		std::ostringstream tmp;
		tmp << "[session] connecting to destination server..."
			<< " at address : " << endpoint << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	server_socket_.connect(endpoint, ec); // todo: add timeout ?
	if (ec) return 0;
	bind_port_ = server_socket_.local_endpoint().port();
	std::cout << "[session] server connected on local port " << bind_port_ << std::endl;
	{
		std::ostringstream tmp;
		tmp << "[session] server connected on local port " << bind_port_ << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	return bind_port_;
}

void session::client_read()
{
	if (client_socket_.is_open()) {
		client_socket_.async_read_some(ba::buffer(client_buf_),
			boost::bind(&session::client_handle, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
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

void session::server_read()
{
	if (server_socket_.is_open()) {
		server_socket_.async_read_some(ba::buffer(server_buf_),
			boost::bind(&session::server_handle, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
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

void session::client_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Client EOF." << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
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
		{
			std::ostringstream tmp;
			tmp << bind_port_
				<< "no bytes transferred, closing connection...\n";
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
	}
}

void session::server_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Server EOF." << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
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
		{
			std::ostringstream tmp;
			tmp << "[" << bind_port_
				<< "] no bytes transferred...\n";
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
	}
}

bool session::writeToSocket(ba::ip::tcp::socket& socket, barray buffer, size_t len, bool isServer)
{
	std::string target = isServer ? "server" : "client"; // todo: optimise
	{
		std::ostringstream tmp;
		tmp << "["
			<< bind_port_
			<< "] " << "Sending " << len << " bytes to " << target << std::endl;
		if (isServer)
		{
			log_stat(server_socket_.remote_endpoint(), client_socket_.remote_endpoint(), len, TO_SERVER);
		}
		else
		{			
			log_stat(server_socket_.remote_endpoint(), client_socket_.remote_endpoint(), len, TO_CLIENT);
		}
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	bs::error_code ec;
	ba::write(socket, ba::buffer(buffer, len), ec);
	if (ec) {
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << ec.what() << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
		return false;
	}
	{
		std::ostringstream tmp;
		tmp << "["
			<< bind_port_
			<< "] Complete!\n";
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	
	return true;
}

void session::close()
{
	{
		std::ostringstream tmp;
		tmp << "[" << bind_port_ << "] Closing sockets..." << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	try {
		client_socket_.close();
		server_socket_.close();
	}
	catch (const bs::system_error& e)
	{
		{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Error occurred during closing: " << e.what() << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
	}
}
