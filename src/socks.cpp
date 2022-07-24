#include "socks.h"
#include "Logger.h"
#include "session.h"
#include "string_utils.h"

void socks::close()
{
#ifdef STAT
	if (_id != 0) {
		try {
			proxy::stat::db_service::getInstance().close(_id);
			log(TRACE_LOG) << "Closed session " << _id << " in database";
		}
		catch (const db_exception& er) {
			log(ERROR_LOG) << er.what();
		}
		_id = 0;
	}
#endif
}

void socks::write_stat(size_t bytes, bool isServer)
{
#ifdef STAT
	if (_id == 0) {
		log(ERROR_LOG) << "id is 0";
		return;
	}

	using namespace proxy::stat;
	try {
		db_service::getInstance().update(_id, bytes, isServer ? Dest::TO_SERVER : Dest::TO_CLIENT);
	}
	catch (const db_exception& er) {
		log(ERROR_LOG) << er.what();
	}
#endif
}

bool socks::createRecord()
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
		_id = db_service::getInstance().create(s);
	}
	catch (const db_exception& ex) {
		log(ERROR_LOG) << "Database record was not created for session:" << ex.what() << "\n";
		return false;
	}
#endif
	return true;
}

bool socks::checkVersion()
{
	bs::error_code ec;
	int ver = _session->readByte(ec);
	if (checkError(ec))
		return false;

	if (ver != _socks_ver)
	{
		log(ERROR_LOG) << "Invalid version: " << ver << std::endl;
		return false;
	}
	return true;
}

bool socks::checkError(bs::error_code& ec)
{
	if (ec)
	{
		log(ERROR_LOG) << "Error occured in socks: " << ec.what();
		return true;
	}
	return false;
}

bool socks::readPort()
{
	bs::error_code ec;
	_serverPort = _session->readByte(ec);
	if (checkError(ec)) return false;
	_serverPort = ((int)_serverPort << 8) | ((int)_session->readByte(ec));
	if (checkError(ec)) return false;
	return true;
}

std::string socks::readIPV4()
{
	bs::error_code ec;
	bvec dstAdd;
	dstAdd.resize(4);
	_session->readBytes(dstAdd, ec);
	if (checkError(ec)) return {};
	return string_utils::formIpAddressString(dstAdd);
}
