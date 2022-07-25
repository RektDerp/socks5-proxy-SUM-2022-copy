#include "session.h"
#include "server.h"
#include "socks4.h"
#include "socks5.h"

TcpSession::TcpSession(TcpServer* server, ba::io_context& io_context, size_t bufferSizeKB):
	_server(server),
	_io_context(io_context), 
	_client_socket(io_context), _server_socket(io_context),
	_socks(nullptr),
	_client_buf(),
	_server_buf()
{
	_client_buf.resize(bufferSizeKB * 1024);
	_server_buf.resize(bufferSizeKB * 1024);
}

TcpSession::~TcpSession()
{
	close();
}

void TcpSession::start()
{
	log(TRACE_LOG) << "IP of connected client: " << _client_socket.remote_endpoint().address() << ":"
		<< _client_socket.remote_endpoint().port();
	if (createProxy()) {
		if (_socks->init()) {
			log(TRACE_LOG) << "[" << _bind_port << "] Starting session...";
			client_read();
			server_read();
		}
	}
}

bool TcpSession::createProxy()
{
	bs::error_code ec;
	int socks_ver = readByte(ec);
	if (ec) {
		log(ERROR_LOG) << "There was an error during reading socks ver: "
			<< ec.message();
		return false;
	}

	switch (socks_ver) {
	case SOCKS5_VER:
		_socks = std::make_unique<Socks5>(this);
		break;
	case SOCKS4_VER:
		_socks = std::make_unique<Socks4>(this);
		break;
	default:
		log(ERROR_LOG) << "Invalid version: " << socks_ver;
		return false;
	}
	return true;
}

// todo add timeout
unsigned char TcpSession::readByte(bs::error_code& ec)
{
	boost::asio::read(_client_socket, ba::buffer(_client_buf),
		boost::asio::transfer_exactly(1), ec);
	return _client_buf[0];
}

void TcpSession::readBytes(bvec& vec, bs::error_code& ec)
{
	boost::asio::read(_client_socket, ba::buffer(vec), ec);
}

void TcpSession::writeBytes(const bvec& bytes, bs::error_code& ec)
{
	ba::write(_client_socket, ba::buffer(bytes), ec);
}

unsigned short TcpSession::connect(ba::ip::tcp::resolver::query& query, bs::error_code& ec)
{
	log(TRACE_LOG) << "[session] connecting to destination server..." << " at address : "
		<< query.host_name() << " " << query.service_name();
	using namespace ba::ip;
	tcp::resolver resolver(_io_context);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, ec);
	if (ec) return 0;
	tcp::resolver::iterator end;
	ec = ba::error::host_not_found;
	while (ec && endpoint_iterator != end)
	{
		_server_socket.close(ec);
		_server_socket.connect(*endpoint_iterator++, ec);
	}
	if (ec) return 0;
	_bind_port = _server_socket.local_endpoint().port();
	log(TRACE_LOG) << "[session] Connected to " << _server_socket.remote_endpoint().address() << ":"
		<< _server_socket.remote_endpoint().port();
	log(TRACE_LOG) << "[session] server connected on local port " << _bind_port;
	return _bind_port;
}

void TcpSession::client_read()
{
	if (_client_socket.is_open()) {
		_client_socket.async_read_some(ba::buffer(_client_buf),
			boost::bind(&TcpSession::client_handle, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		log(TRACE_LOG) << "["
			<< _bind_port
			<< "] Stopped reading - client socket is closed.";
	}
}

void TcpSession::server_read()
{
	if (_server_socket.is_open()) {
		_server_socket.async_read_some(ba::buffer(_server_buf),
			boost::bind(&TcpSession::server_handle, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		log(TRACE_LOG) << "["
			<< _bind_port
			<< "] Stopped reading - server socket is closed.";
	}
}

void TcpSession::client_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		log(TRACE_LOG) << "[" << _bind_port << "] Client EOF.";
		return;
	}
	else if (error.value() > 0)
	{
		log(ERROR_LOG) << "["
			<< _bind_port
			<< "] " << "error occured while reading client: "
			<< error.what();
		return;
	}
	
	if (bytes_transferred > 0)
	{
		if (writeToSocket(_server_socket, _client_buf, bytes_transferred, true))
		{
			client_read();
		}
	}
	else {
		log(TRACE_LOG) << "[" << _bind_port << "] no bytes transferred, closing connection...";
	}
}

void TcpSession::server_handle(const bs::error_code& error, size_t bytes_transferred)
{
	if (error.value() == ba::error::eof)
	{
		log(TRACE_LOG) << "[" << _bind_port << "] Server EOF.";
		return;
	}
	else if (error.value() > 0)
	{
		log(ERROR_LOG) << "["
			<< _bind_port
			<< "] " << "error occured while reading server: "
			<< error.what();
		return;
	}

	if (bytes_transferred > 0)
	{
		if (writeToSocket(_client_socket, _server_buf, bytes_transferred, false))
		{
			server_read();
		}
	}
	else {
		log(TRACE_LOG) << "[" << _bind_port << "] no bytes transferred...";
	}
}

bool TcpSession::writeToSocket(ba::ip::tcp::socket& socket, bvec& buffer, size_t len, bool isServer)
{
	std::string target = isServer ? "server" : "client"; // todo: optimise
	log(DEBUG_LOG) << "["
			<< _bind_port
			<< "] " << "Sending " << len << " bytes to " << target;
	bs::error_code ec;
	ba::write(socket, ba::buffer(buffer, len), ec);
	if (ec) {
		log(ERROR_LOG) << "[" << _bind_port << "] Error occurred during writing: " << ec.what();
		return false;
	}
	_socks->write_stat(len, isServer);
	return true;
}

void TcpSession::close()
{
	log(DEBUG_LOG) << "[" << _bind_port << "] Closing sockets...";
	try {
		_client_socket.close();
	}
	catch (const bs::system_error& e)
	{
		log(ERROR_LOG) << "[" << _bind_port << "] Error occurred during closing client socket: " << e.what();
	}
	try {
		_server_socket.close();
	}
	catch (const bs::system_error& e)
	{
		log(ERROR_LOG) << "[" << _bind_port << "] Error occurred during closing server socket: " << e.what();
	}
	if (_socks != nullptr) _socks->close();
	_server->_sessions--;
}
