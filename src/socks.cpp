#include "socks.h"
#include "Logger.h"
#include "session.h"
#include "string_utils.h"

void Socks::close()
{
#ifdef STAT
	if (_id != 0) {
		try {
			proxy::stat::DatabaseService::getInstance().close(_id);
			log(TRACE_LOG) << "Closed session " << _id << " in database";
		}
		catch (const DatabaseException& er) {
			log(ERROR_LOG) << er.what();
		}
		_id = 0;
	}
#endif
}

void Socks::write_stat(size_t bytes, bool isServer)
{
#ifdef STAT
	if (_id == 0) {
		log(ERROR_LOG) << "Cannot write statistics: id of session is 0";
		return;
	}

	using namespace proxy::stat;
	try {
		DatabaseService::getInstance().update(_id, bytes, isServer ? Dest::TO_SERVER : Dest::TO_CLIENT);
	}
	catch (const DatabaseException& er) {
		log(ERROR_LOG) << er.what();
	}
#endif
}

bool Socks::createRecord()
{
#ifdef STAT
	using namespace proxy::stat;
	proxy::stat::Session s;
	s.user = _username;
	s.src_addr = _session->socket().remote_endpoint().address().to_string();
	s.src_port = std::to_string(_session->socket().remote_endpoint().port());
	s.dst_addr = _dstAddress;
	s.dst_port = std::to_string(_dstPort);
	try {
		_id = DatabaseService::getInstance().create(s);
	}
	catch (const DatabaseException& ex) {
		log(ERROR_LOG) << "Failed to create record in database: " << ex.what();
		return false;
	}
#endif
	return true;
}

bool Socks::checkVersion()
{
	bs::error_code ec;
	int ver = _session->readByte(ec);
	if (checkError(ec))
		return false;

	if (ver != _socks_ver)
	{
		log(ERROR_LOG) << "Invalid version: " << ver;
		return false;
	}
	return true;
}

bool Socks::checkError(bs::error_code& ec)
{
	if (ec)
	{
		log(ERROR_LOG) << "Error occured in socks: " << ec.what();
		return true;
	}
	return false;
}

bool Socks::readPort()
{
	bs::error_code ec;
	_dstPort = _session->readByte(ec);
	if (checkError(ec)) return false;
	_dstPort = ((int)_dstPort << 8) | ((int)_session->readByte(ec));
	if (checkError(ec)) return false;
	return true;
}

std::string Socks::readIPV4()
{
	bs::error_code ec;
	bvec dstAdd;
	dstAdd.resize(4);
	_session->readBytes(dstAdd, ec);
	if (checkError(ec)) return {};
	return string_utils::formIpAddressString(dstAdd);
}
