#include "socks5_impl.h"
#include "string_utils.h"
#include "session.h"

using string_utils::to_string;
using string_utils::formIpAddressString;

bool socks5_impl::init()
{
	bs::error_code ec;

	/*  +---- + ---------- + ---------- +
		| VER |  NMETHODS  |   METHODS  |
		+---- + ---------- + ---------- +
		|  1  |      1     |  1 to 255  |
		+---- + ---------- + ---------- +  */

	unsigned char ver = session_->readByte(ec);
	if (checkError(ec))
		return false;
	
	if (ver != SOCKS_VER)
	{
		std::cerr << "Invalid version" << std::endl;
		// todo send message
		return false;
	}

	unsigned char nMethods = session_->readByte(ec);
	if (checkError(ec))
		return false;

	bvec methods;
	methods.resize(nMethods);
	session_->readBytes(methods, ec);
	if (checkError(ec))
		return false;

	unsigned char serverMethod = AUTH_FLAG ? USERNAME_PASSWORD : NO_AUTH_REQ;

	if (std::find(methods.begin(), methods.end(), serverMethod) == methods.end())
	{
		bvec response;
		response.push_back(SOCKS_VER);
		response.push_back(NO_ACC_METHODS);
		session_->writeBytes(response, ec); // ec ignored
		return false;
	}

	methods.clear();
	methods.push_back(SOCKS_VER);
	methods.push_back(serverMethod);

	session_->writeBytes(methods, ec);
	if (checkError(ec))
		return false;

	if (AUTH_FLAG)
	{
		if (!auth())
			return false;
	}

	/*  +----+-----+-------+------+----------+----------+
		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+ */

	// todo sent response code in case of failure

	ver = session_->readByte(ec);
	if (ver != SOCKS_VER) return false;
	if (checkError(ec)) return false;
	unsigned char cmd = session_->readByte(ec);
	if (checkError(ec)) return false;
	session_->readByte(ec); // rsv
	if (checkError(ec)) return false;
	unsigned char atyp = session_->readByte(ec);
	if (checkError(ec)) return false;

	std::string raw_ip_address;
	if (atyp == ATYP::IPV4) {
		bvec dstAdd;
		dstAdd.resize(4);
		session_->readBytes(dstAdd, ec);
		if (checkError(ec))
			return false;

		raw_ip_address = formIpAddressString(dstAdd);
	}
	else if (atyp == ATYP::IPV6) {
		return false;
		// todo to be implemented
	}
	else {
		return false;
		// wrong atyp - send response
	}

	unsigned short server_port = session_->readByte(ec);
	if (checkError(ec)) return false;
	server_port = ((int)server_port << 8) | ((int)session_->readByte(ec));
	if (checkError(ec)) return false;
	ba::ip::address ip_address = ba::ip::address::from_string(raw_ip_address, ec);
	if (checkError(ec)) return false;
	ba::ip::tcp::endpoint ep(ip_address, server_port);

	unsigned short bind_port = 0;
	if (cmd == CMD::CONNECT)
	{
		bind_port = session_->connect(ep, ec);
		if (checkError(ec))
			return false;
	}
	else if (cmd == CMD::BIND)
	{
		return false;
		// todo to be implemented
	}
	else if (cmd == CMD::UDP_ASSOCIATE)
	{
		return false;
		// todo to be implemented
	}
	else {
		return false;
		// wrong cmd code - send response
	}

	/*  +----+-----+-------+------+----------+----------+
		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+*/

	bvec response;
	response.push_back(SOCKS_VER);
	response.push_back(SUCCEEDED);
	response.push_back(RSV);
	response.push_back(atyp);
	if (atyp == IPV4) {
		// todo do we need to resolve server address?
		response.push_back(0);
		response.push_back(0);
		response.push_back(0);
		response.push_back(0);
	}
	else if (atyp == IPV6) {
		return false;
		// todo
	}
	response.push_back((bind_port & 0xFF) >> 8);
	response.push_back(bind_port & 0x00FF);

	session_->writeBytes(response, ec);

	if (checkError(ec)) {
		return false;
	}
	return true;
}

bool socks5_impl::auth()
{
	/*
	+---- + ------ + ---------- + ------ + ---------- +
	| VER |  ULEN  |    UNAME   |  PLEN  |   PASSWD   |
	+---- + ------ + ---------- + ------ + ---------- +
	|  1  |    1   |  1 to 255  |    1   |  1 to 255  |
	+---- + ------ + ---------- + ------ + ---------- +
	*/
	bs::error_code ec;

	unsigned int ver = session_->readByte(ec);
	if (checkError(ec) || ver != AUTH_VER)
		return false;

	unsigned short ulen = session_->readByte(ec);
	if (checkError(ec) || ulen == 0)
		return false;

	bvec uname;
	uname.resize(ulen);
	session_->readBytes(uname, ec);
	if (checkError(ec))
		return false;

	unsigned char plen = session_->readByte(ec);
	if (checkError(ec))
		return false;
	bvec passwd;
	passwd.resize(plen);
	session_->readBytes(passwd, ec);
	if (checkError(ec))
		return false;

	std::string userString = string_utils::to_string(uname);
	std::string passString = string_utils::to_string(passwd);

	std::cout << "user: '" << userString << "', pass: '" << passString << "'" << std::endl;

	bool success = true;
	if (USER != userString || PASSWORD != passString)
		return false;

	bvec response;
	response.push_back(AUTH_VER);
	response.push_back(success ? AUTH_STATUS::SUCCESS : AUTH_STATUS::DENY);
	session_->writeBytes(response, ec);
	if (checkError(ec)) {
		return false;
	}
	return true;
}

bool socks5_impl::checkError(bs::error_code& ec)
{
	if (ec)
	{
		{
			std::ostringstream tmp;
			tmp << ec.what() << std::endl;
			CPlusPlusLogging::LOG_ERROR(tmp);
		}
		std::cerr << ec.what() << std::endl;
		return true;
	}
	return false;
}