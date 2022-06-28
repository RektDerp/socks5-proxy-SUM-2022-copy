#include "server.h"

tcp_server::tcp_server(ba::io_context& io_context): 
	io_context_(io_context),
	acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), listen_port))
{
	start_accept();
}

void tcp_server::start_accept()
{
	std::cout << "[server] waiting for new client... " << acceptor_.local_endpoint() << std::endl;
	tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "[server] client connected" << std::endl;
		new_connection->start();
	}

	start_accept();
}