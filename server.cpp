//#include <boost/asio/ts/buffer.hpp>
//#include <boost/asio/ts/internet.hpp>

#include "server.h"


tcp_server::tcp_server(boost::asio::io_context& io_context): 
	io_context_(io_context),
	acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), listen_port))
{
	start_accept();
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
{
	if (!error)
	{
		new_connection->start();
	}

	start_accept();
}


void tcp_server::start_accept()
{
	tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

	std::cout << "waiting for new client... " << acceptor_.local_endpoint() << std::endl;
	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

