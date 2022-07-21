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
	log("trace") << "IP of connected client: " << client_socket_.remote_endpoint().address() << ":"
		<< client_socket_.remote_endpoint().port() << "\n";
	if (impl_->init()) {
		log("trace") << "[" << bind_port_ << "] Starting session...\n";
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
	log("trace") << "[session] connecting to destination server..." << " at address : "
		<< query.host_name() << " " << query.service_name() << "\n";
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
	log("trace") << "[session] server connected on local port " << bind_port_ << "\n";
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
		log("trace") << "["
			<< bind_port_
			<< "] Stopped reading - client socket is closed.\n";
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
		log("trace") << "["
			<< bind_port_
			<< "] Stopped reading - server socket is closed.\n";
	}
}

void session::client_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		log("trace") << "[" << bind_port_ << "] Client EOF.\n";
		return;
	}
	else if (error.value() > 0)
	{
		log("error") << "["
			<< bind_port_
			<< "] " << "error occured while reading client: "
			<< error.what() << "\n";
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
		log("trace") << "[" << bind_port_ << "] no bytes transferred, closing connection...\n";
	}
}

void session::server_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		log("trace") << "[" << bind_port_ << "] Server EOF.\n";
		return;
	}
	else if (error.value() > 0)
	{
		log("error") << "["
			<< bind_port_
			<< "] " << "error occured while reading server: "
			<< error.what() << "\n";
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
		log("trace") << "[" << bind_port_ << "] no bytes transferred...\n";
	}
}

bool session::writeToSocket(ba::ip::tcp::socket& socket, bvec& buffer, size_t len, bool isServer)
{
	std::string target = isServer ? "server" : "client"; // todo: optimise
	log("debug") << "["
			<< bind_port_
			<< "] " << "Sending " << len << " bytes to " << target << "\n";
	bs::error_code ec;
	ba::write(socket, ba::buffer(buffer, len), ec);
	if (ec) {
		log("error") << "[" << bind_port_ << "] " << ec.what() << "\n";
		return false;
	}
	impl_->write_stat(len, isServer);
	return true;
}

void session::close()
{
	log("debug") << "[" << bind_port_ << "] Closing sockets..." << "\n";
	try {
		client_socket_.close();
	}
	catch (const bs::system_error& e)
	{
		log("error") << "[" << bind_port_ << "] Error occurred during closing client socket: " << e.what() << "\n";
	}
	try {
		server_socket_.close();
	}
	catch (const bs::system_error& e)
	{
		log("error") << "[" << bind_port_ << "] Error occurred during closing server socket: " << e.what() << "\n";
	}
	impl_->close();
	_server->_sessions--;
}
