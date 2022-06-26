#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/array.hpp>
#include <bitset>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <chrono>

namespace ba = boost::asio;
namespace bs = boost::system;

const unsigned short tcp_port = 8888;
const unsigned short bind_port = 10000;
const ba::ip::tcp ipv4 = ba::ip::tcp::v4();

struct ver_packet
{
	char ver;
	char nMethod;
	char Method[255];
};

const size_t BUFFER_LEN = 8192;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_context &io_context)
	{
		return pointer(new tcp_connection(io_context));
	}

	ba::ip::tcp::socket &socket()
	{
		return client_socket_;
	}

	void init()
	{
		std::cout << "started negotiations on " << client_socket_.local_endpoint() << std::endl;
		bs::error_code error;

		size_t n = client_socket_.read_some(ba::buffer(client_buf_));
		std::cout << "read " << n << " bytes\n";
		ver_packet packet;
		packet.ver = client_buf_[0];
		packet.nMethod = client_buf_[1];

		std::cout << (int)packet.ver << "  " << (int)packet.nMethod << "\n";

		for (int i = 0; i < packet.nMethod; i++)
		{
			packet.Method[i] = client_buf_[2 + i];
			std::cout << (int)packet.Method[i] << std::endl;
		}

		client_buf_[0] = packet.ver;
		client_buf_[1] = 0;
		ba::write(client_socket_, ba::buffer(client_buf_, 2));

		/*+----+-----+-------+------+----------+----------+
			|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
			+----+-----+-------+------+----------+----------+
			| 1  |  1  | X'00' |  1   | Variable |    2     |
			+----+-----+-------+------+----------+----------+*/

		n = client_socket_.read_some(ba::buffer(client_buf_));
		std::cout << "read " << n << " bytes\n";
		for (int i = 0; i < 10; i++)
		{
			std::cout << (int)client_buf_[i] << std::endl;
		}

		std::string raw_ip_address =
				std::to_string((int)client_buf_[4]) +
				'.' +
				std::to_string((int)client_buf_[5]) +
				'.' +
				std::to_string((int)client_buf_[6]) +
				'.' +
				std::to_string((int)client_buf_[7]);

		unsigned short server_port = ((int)client_buf_[8] << 8) | (int)client_buf_[9];
		std::cout << "dst address: " << raw_ip_address << ':' << server_port << std::endl;

		std::cout << "connecting to server..." << std::endl;

		boost::system::error_code ec;
		ba::ip::address ip_address = ba::ip::address::from_string(raw_ip_address, ec);

		if (ec.value() != 0)
		{
			std::cout
					<< "Failed to parse the IP address. Error code = "
					<< ec.value() << ". Message: " << ec.message();
			throw ec;
		}

		ba::ip::tcp::endpoint ep(ip_address, server_port);
		std::cout << "at address: " << ep << std::endl;
		server_socket_.connect(ep);
		std::cout << "server connected." << std::endl;

		/*+----+-----+-------+------+----------+----------+
			|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
			+----+-----+-------+------+----------+----------+
			| 1  |  1  | X'00' |  1   | Variable |    2     |
			+----+-----+-------+------+----------+----------+*/

		// client_buf_[0] = packet.ver;
		client_buf_[1] = 0;
		client_buf_[4] = 127;
		client_buf_[5] = 0;
		client_buf_[6] = 0;
		client_buf_[7] = 1;
		client_buf_[8] = (bind_port & 0xFF00) >> 8;
		client_buf_[9] = bind_port & 0x00FF;

		for (int i = 0; i < 10; i++)
		{
			std::cout << (unsigned short)client_buf_[i] << std::endl;
		}

		ba::write(client_socket_, ba::buffer(client_buf_, 10));
	}

	void start()
	{
		init();

		client_read();
		server_read();
	}

private:
	tcp_connection(boost::asio::io_context &io_context)
			: client_socket_(io_context), server_socket_(io_context)
	{
	}

	void client_read()
	{
		client_socket_.async_read_some(ba::buffer(client_buf_),
																	 boost::bind(&tcp_connection::client_read_handle, shared_from_this(),
																							 boost::asio::placeholders::error,
																							 boost::asio::placeholders::bytes_transferred));
	}

	void client_read_handle(const boost::system::error_code &error, size_t bytes_transferred)
	{
		std::cout << "handling client...\n";
		if (error)
		{
			std::cout << error.what() << std::endl;
			return;
		}

		if (bytes_transferred > 0)
		{
			std::cout << "Sending " << bytes_transferred << " bytes to client\n";
			ba::write(server_socket_, ba::buffer(client_buf_, bytes_transferred));
			std::cout << "complete!\n";
		}
		client_read();
	}

	void server_read()
	{
		server_socket_.async_read_some(ba::buffer(server_buf_),
																	 boost::bind(&tcp_connection::server_read_handle, shared_from_this(),
																							 boost::asio::placeholders::error,
																							 boost::asio::placeholders::bytes_transferred));
	}

	void server_read_handle(const boost::system::error_code &error, size_t bytes_transferred)
	{
		std::cout << "handling server...\n";
		if (error)
		{
			std::cout << error.what() << std::endl;
			return;
		}

		if (bytes_transferred > 0)
		{
			std::cout << "Sending " << bytes_transferred << " bytes to client\n";
			ba::write(client_socket_, ba::buffer(server_buf_, bytes_transferred));
			std::cout << "complete!\n";
		}
		server_read();
	}

	ba::ip::tcp::socket client_socket_;
	ba::ip::tcp::socket server_socket_;
	boost::array<unsigned char, BUFFER_LEN> client_buf_{};
	boost::array<unsigned char, BUFFER_LEN> server_buf_{};
};

class tcp_server
{
public:
	tcp_server(boost::asio::io_context &io_context)
			: io_context_(io_context),
				acceptor_(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), tcp_port))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		tcp_connection::pointer new_connection = tcp_connection::create(io_context_);

		std::cout << "waiting for new client... " << acceptor_.local_endpoint() << std::endl;
		acceptor_.async_accept(new_connection->socket(),
													 boost::bind(&tcp_server::handle_accept, this, new_connection,
																			 boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code &error)
	{
		if (!error)
		{
			new_connection->start();
		}

		start_accept();
	}

	boost::asio::io_context &io_context_;
	ba::ip::tcp::acceptor acceptor_;
};

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	ba::io_context context;
	std::thread thrContext = std::thread([&]()
																			 { context.run(); });
	ba::io_context::work idlework(context);
	try
	{
		tcp_server server(context);
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
	}
	catch (std::exception &er)
	{
		std::cerr << er.what() << std::endl;
	}

	context.stop();
	if (thrContext.joinable())
		thrContext.join();

	/*
		попытался разобраться но пока четно
		несколько версий
		1. оно не может закрыть поток и вызывает аборт

		2. чаще всего это происходит если ты хочешь обновить страничку или вернуться на страничку на которой уже бывал
		я не очень уверен но может нужен какой-то механизм разрыва соеденения

		после приблезительно сотни тестов сайты кроме главной странички вики перестали открываться(с ходом она тоже перестала открывать)


	*/
	return 0;
}