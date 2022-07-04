#include "server.h"

tcp_server::tcp_server(ba::io_context& io_context, unsigned short port): 
	io_context_(io_context),
	acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port))
{
	start_accept();
}

void tcp_server::start_accept()
{
	std::cout << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
	tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
	{
		std::ostringstream tmp;
		tmp << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
		CPlusPlusLogging::LOG_TRACE(tmp);
	}
	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "[server] client connected" << std::endl;
		{
			std::ostringstream tmp;
			tmp << "[server] client connected" << std::endl;
			CPlusPlusLogging::LOG_TRACE(tmp);
		}
		std::thread newThread(&tcp_connection::start, new_connection);
		newThread.detach();
	}

	start_accept();
}