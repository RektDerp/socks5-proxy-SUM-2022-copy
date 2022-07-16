#include "server.h"

tcp_server::tcp_server(ba::io_context& io_context, unsigned short port, size_t bufferSizeKB): 
	io_context_(io_context),
	acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)),
	bufferSizeKB_(bufferSizeKB),
	_pool(100)
{
	std::cout << "Server starting on port " << port
		<< " with buffer size " << bufferSizeKB << " KB." << std::endl;
	start_accept();
}

void tcp_server::start_accept()
{
	std::cout << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
	session::pointer new_connection = session::create(io_context_, bufferSizeKB_);
	/*{
		std::ostringstream tmp;
		tmp << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}*/
	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(session::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		//std::cout << "[server] client connected" << std::endl;
		/*{
			std::ostringstream tmp;
			tmp << "[server] client connected" << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}*/
		ba::post(_pool, [new_connection]() { new_connection->start(); });
	}

	start_accept();
}