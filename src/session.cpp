#include "session.h"
#include "server.h"
#include "socks5_impl.h"

session::session(tcp_server* server, ba::io_context& io_context, size_t bufferSizeKB):
	_server(server),
	io_context_(io_context), 
	client_socket_(io_context), server_socket_(io_context),
	impl_(new socks5_impl(this)),
	client_buf_(),
	server_buf_()
{
	client_buf_.resize(bufferSizeKB * 1024);
	server_buf_.resize(bufferSizeKB * 1024);
}

session::~session()
{
	close();
	delete impl_;
}

void session::start()
{
	if (impl_->init()) {
		{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Starting session...\n";
			LOG_TRACE(tmp);
		}
		client_read();
		server_read();
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

unsigned short session::connect(ba::ip::tcp::resolver::query& query, bs::error_code& ec)
{
	//std::cout << "[session] connecting to destination server..." << " at address : "
	//	<< query.host_name() << " " << query.service_name() << std::endl;
	//{
	//	std::ostringstream tmp;
	//	tmp << "[session] connecting to destination server..." << " at address : "
	//	<< query.host_name() << " " << query.service_name() << std::endl;
	//	CPlusPlusLogging::LOG_TRACE(tmp);
	//}
	using namespace ba::ip;
	tcp::resolver resolver(io_context_);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::resolver::iterator end;
	ec = ba::error::host_not_found;
	while (ec && endpoint_iterator != end)
	{
		server_socket_.close(ec);
		server_socket_.connect(*endpoint_iterator++, ec);
	}
	if (ec) return 0;
	bind_port_ = server_socket_.local_endpoint().port();
	//{
	//	std::ostringstream tmp;
	//	tmp << "[session] server connected on local port " << bind_port_ << std::endl;
	//	CPlusPlusLogging::LOG_TRACE(tmp);
	//}
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
		/*{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << " Stopped reading - client socket is closed."
				<< std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
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
		/*{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] Stopped reading - server socket is closed."
				<< std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
	}
}

void session::client_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		/*{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Client EOF." << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}*/
		return;
	}
	else if (error.value() > 0)
	{
		/*{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << "error occured while reading client: "
				<< error.what() << std::endl;;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}*/
		return;
	}
	
	if (bytes_transferred > 0)
	{
		if (writeToSocket(server_socket_, client_buf_, bytes_transferred, true))
		{
			client_read();
		}
	}
	else {
		/*{
			std::ostringstream tmp;
			tmp << bind_port_
				<< "no bytes transferred, closing connection...\n";
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
	}
}

void session::server_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		/*{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Server EOF." << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
		return;
	}
	else if (error.value() > 0)
	{
		/*{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << "error occured while reading server: "
				<< error.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}*/
		return;
	}

	if (bytes_transferred > 0)
	{
		if (writeToSocket(client_socket_, server_buf_, bytes_transferred, false))
		{
			server_read();
		}
	}
	else {
		/*{
			std::ostringstream tmp;
			tmp << "[" << bind_port_
				<< "] no bytes transferred...\n";
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
	}
}

bool session::writeToSocket(ba::ip::tcp::socket& socket, bvec& buffer, size_t len, bool isServer)
{
	std::string target = isServer ? "server" : "client"; // todo: optimise
	{
		std::ostringstream tmp;
		tmp << "["
			<< bind_port_
			<< "] " << "Sending " << len << " bytes to " << target << std::endl;
		LOG_TRACE(tmp);
	}
	bs::error_code ec;
	ba::write(socket, ba::buffer(buffer, len), ec);
	if (ec) {
		std::cerr << "["
			<< bind_port_
			<< "] " << ec.what() << std::endl;
		{
			std::ostringstream tmp;
			tmp << "["
				<< bind_port_
				<< "] " << ec.what() << std::endl;
			LOG_TRACE(tmp);
		}
		return false;
	}
	/*{
		std::ostringstream tmp;
		tmp << "["
			<< bind_port_
			<< "] Complete!\n";
		CPlusPlusLogging::LOG_TRACE(tmp);
	}*/

	impl_->write_stat(len, isServer);
	return true;
}

void session::close()
{
	/*{
		std::ostringstream tmp;
		tmp << "[" << bind_port_ << "] Closing sockets..." << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}*/
	try {
		client_socket_.close();
	}
	catch (const bs::system_error& e)
	{

	}
	try {
		server_socket_.close();
	}
	catch (const bs::system_error& e)
	{
		/*{
			std::ostringstream tmp;
			tmp << "[" << bind_port_ << "] Error occurred during closing: " << e.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}*/
	}
	impl_->close();
	_server->_sessions--;
}
