#include "socks5_impl.h"
#include "string_utils.h"
#include "session.h"
#ifdef STAT
#include "stat_db_service.h"
#endif
using string_utils::to_string;
using string_utils::formIpAddressString;

bool socks5_impl::init()
{
	if (!readMethodRequest())
		return false;
	
	if (_authFlag && !auth())
		return false;
		

	if (!readCommandRequest())
		return false;

	ba::ip::tcp::resolver::query query(_dstAddress, std::to_string(_serverPort));

	if (_cmd == CMD::CONNECT)
	{
		if (!connect(query))
			return false;
	}
	else if (_cmd == CMD::BIND)
	{
		sendErrorResponse(COMMAND_NOT_SUPP);
		return false;
	}
	else if (_cmd == CMD::UDP_ASSOCIATE)
	{
		sendErrorResponse(COMMAND_NOT_SUPP);
		return false;
	}
	else {
		sendErrorResponse(COMMAND_NOT_SUPP);
		return false;
	}
	
	if (!createRecord())
		return false;

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

	unsigned int ver = _session->readByte(ec);
	if (checkError(ec) || ver != AUTH_VER)
		return false;

	unsigned short ulen = _session->readByte(ec);
	if (checkError(ec) || ulen == 0)
		return false;

	bvec uname;
	uname.resize(ulen);
	_session->readBytes(uname, ec);
	if (checkError(ec))
		return false;

	unsigned char plen = _session->readByte(ec);
	if (checkError(ec))
		return false;
	bvec passwd;
	passwd.resize(plen);
	_session->readBytes(passwd, ec);
	if (checkError(ec))
		return false;

	_username = string_utils::to_string(uname);
	std::string passString = string_utils::to_string(passwd);

	bool success = LogConfigReader::getInstance()->hasUser(_username, passString);
	if (!success)
		log(ERROR_LOG) << "Wrong credentials: " << _username << ":" << passString << "\n";
	bvec response;
	response.push_back(AUTH_VER);
	response.push_back(success ? AUTH_STATUS::SUCCESS : AUTH_STATUS::DENY);
	_session->writeBytes(response, ec);
	if (checkError(ec)) {
		return false;
	}
	return success;
}

bool socks5_impl::checkError(bs::error_code& ec)
{
	if (ec)
	{
		log(ERROR_LOG) << ec.what() << "\n";
		return true;
	}
	return false;
}

void socks5_impl::write_stat(size_t bytes, bool isServer)
{
#ifdef STAT
	if (id_ == 0) {
		log(ERROR_LOG) << "id is 0\n";
		return;
	}

	using namespace proxy::stat;
	try {
		db_service::getInstance().update(id_, bytes, isServer ? Dest::TO_SERVER : Dest::TO_CLIENT);
	}
	catch (const db_exception& er) {
		log(ERROR_LOG) << er.what() << "\n";
	}
#endif
}

void socks5_impl::close()
{
#ifdef STAT
	if (id_ != 0) {
		try {
			proxy::stat::db_service::getInstance().close(id_);
			log(TRACE_LOG) << "Closed session " << id_ << " in database\n";
		}
		catch (const db_exception& er) {
			log(ERROR_LOG) << er.what() << "\n";
		}
		id_ = 0;
	}
#endif
}

void socks5_impl::sendErrorResponse(REP responseCode)
{
	bvec response;
	response.push_back(SOCKS_VER);
	response.push_back(responseCode);
	response.push_back(RSV);
	response.push_back(IPV4);
	response.resize(10); // length of response
	bs::error_code ec;
	_session->writeBytes(response, ec); // ec ignored
}

std::string socks5_impl::readAddress(unsigned char atyp)
{
	bs::error_code ec;
	if (atyp == ATYP::IPV4) {
		bvec dstAdd;
		dstAdd.resize(4);
		_session->readBytes(dstAdd, ec);
		if (checkError(ec)) return {};
		return formIpAddressString(dstAdd);
	}
	else if (atyp == ATYP::DOMAIN_NAME) {
		unsigned char addSize = _session->readByte(ec);
		if (checkError(ec)) return {};
		bvec dstAdd;
		dstAdd.resize(addSize);
		_session->readBytes(dstAdd, ec);
		if (checkError(ec)) return {};
		return to_string(dstAdd);
	}
	else if (atyp == ATYP::IPV6) {
		bvec dstAdd;
		dstAdd.resize(16);
		_session->readBytes(dstAdd, ec);
		if (checkError(ec)) return {};
		return formIpAddressString(dstAdd);
		/*sendErrorResponse(ADDR_TYPE_NOT_SUPP);
		return {};*/
		// todo to be implemented
	}
	sendErrorResponse(ADDR_TYPE_NOT_SUPP);
	return {};
}

bool socks5_impl::checkVersion()
{
	bs::error_code ec;
	unsigned char ver = _session->readByte(ec);
	if (checkError(ec))
		return false;

	if (ver != SOCKS_VER)
	{
		{
			std::ostringstream tmp;
			tmp << "Invalid version: " << (int)ver << std::endl;
			LOG_ERROR(tmp);
		}
		return false;
	}
	return true;
}

bool socks5_impl::checkMethod()
{
	bs::error_code ec;
	unsigned char nMethods = _session->readByte(ec);
	if (checkError(ec))
		return false;
	bvec methods;
	methods.resize(nMethods);
	_session->readBytes(methods, ec);
	if (checkError(ec))
		return false;
	return std::find(methods.begin(), methods.end(), getServerMethod()) != methods.end();
}

bool socks5_impl::sendMethodResponse(METHOD method)
{
	bs::error_code ec;
	bvec response;
	response.push_back(SOCKS_VER);
	response.push_back(method);
	_session->writeBytes(response, ec);
	if (ec) return true;
	return false;
}

bool socks5_impl::readMethodRequest()
{
	/*  +---- + ---------- + ---------- +
		| VER |  NMETHODS  |   METHODS  |
		+---- + ---------- + ---------- +
		|  1  |      1     |  1 to 255  |
		+---- + ---------- + ---------- +  */
	if (!checkVersion())
	{
		sendMethodResponse(METHOD::NO_ACC_METHODS);
		return false;
	}
	if (!checkMethod())
	{
		sendMethodResponse(METHOD::NO_ACC_METHODS);
		return false;
	}	
	return !sendMethodResponse(getServerMethod());
}

bool socks5_impl::readCommandRequest()
{
	/*  +----+-----+-------+------+----------+----------+
		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+ */
	
	bs::error_code ec;
	if (!checkVersion())
	{
		sendErrorResponse(SERVER_FAILURE);
		return false;
	}

	_cmd = _session->readByte(ec);
	if (checkError(ec)) return false;
	_session->readByte(ec); // rsv
	if (checkError(ec)) return false;
	_atyp = _session->readByte(ec);
	if (checkError(ec)) return false;

	_dstAddress = readAddress(_atyp);
	if (_dstAddress.size() == 0) return false;

	_serverPort = _session->readByte(ec);
	if (checkError(ec)) return false;
	_serverPort = ((int) _serverPort << 8) | ((int) _session->readByte(ec));
	if (checkError(ec)) return false;
	return true;
}

bool socks5_impl::createRecord()
{
#ifdef STAT
	using namespace proxy::stat;

	proxy::stat::session s;
	s.user = _username;
	s.src_addr = _session->socket().remote_endpoint().address().to_string();
	s.src_port = std::to_string(_session->socket().remote_endpoint().port());
	s.dst_addr = _dstAddress;
	s.dst_port = std::to_string(_serverPort);

	try {
		id_ = db_service::getInstance().create(s);
	}
	catch (const db_exception& ex) {
		log(ERROR_LOG) << "Database record was not created for session:" << ex.what() << "\n";
		return false;
	}
#endif
	return true;
}

bool socks5_impl::sendCommandResponse(unsigned short bindPort)
{
	/*  +----+-----+-------+------+----------+----------+
		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------+ */

	bvec response;
	response.push_back(SOCKS_VER);
	response.push_back(SUCCEEDED);
	response.push_back(RSV);
	if (_atyp == IPV4 || _atyp == DOMAIN_NAME) {
		response.push_back(IPV4);
		response.push_back(0);
		response.push_back(0);
		response.push_back(0);
		response.push_back(0);
	}
	else if (_atyp == IPV6) {
		response.push_back(IPV6);
		for (size_t i = 0; i < 16; i++)
		{
			response.push_back(0)
		}
		//log(ERROR_LOG) << "IPV6 is not suppored.\n";
		//return false;
	}
	response.push_back((bindPort & 0xFF) >> 8);
	response.push_back(bindPort & 0x00FF);

	bs::error_code ec;
	_session->writeBytes(response, ec);
	if (checkError(ec))
		return false;
	return true;
}

bool socks5_impl::connect(ba::ip::tcp::resolver::query query)
{
	bs::error_code ec;
	_bindPort = _session->connect(query, ec);

	REP reply = SUCCEEDED;
	using namespace ba::error;
	if (ec == host_not_found || ec == host_unreachable)
	{
		reply = HOST_UNREACHABLE;
	}
	else if (ec == connection_aborted)
	{
		reply = CONN_NOT_ALLOWED;
		
	}
	else if (ec == connection_refused)
	{
		reply = CONNECTION_REFUSED;
	}
	else if (ec == timed_out)
	{
		reply = TTL_EXPIRED;
	}
	else if (ec == network_unreachable)
	{
		reply = NETWORK_UNREACH;
	} 
	else if (ec) {
		reply = SERVER_FAILURE;
	}

	if (reply == SUCCEEDED)
	{
		sendCommandResponse(_bindPort);
		return true;
	}
	else
	{
		sendErrorResponse(reply);
		return false;
	}
}
