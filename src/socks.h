#ifndef _SOCKS_H_
#define _SOCKS_H_
#include "proxy_common.h"
#include "stat_db_service.h"

const unsigned char RSV = 0x00;

enum VER {
	SOCKS4_VER = 4,
	SOCKS5_VER = 5
};

enum AUTH_STATUS {
	SUCCESS = 0x0,
	DENY = 0x1
};

enum CMD {
	CONNECT = 0x1,
	BIND = 0x2,
	UDP_ASSOCIATE = 0x3
};

class TcpSession;

class Socks {
protected:
	Socks(TcpSession* const session, VER socks_ver):
		_session(session), _id(0), _socks_ver(socks_ver)
	{}
public:
	virtual bool init() = 0;
	void close();
	void write_stat(size_t bytes, bool isServer);
protected:
	TcpSession* const _session;
	long long _id;
	const VER _socks_ver;
	std::string _dstAddress;
	unsigned short _dstPort;
	unsigned short _bindPort;
	std::string _username;

	bool checkVersion();
	bool checkError(bs::error_code& ec);
	bool createRecord();
	bool readPort();
	std::string readIPV4();
private:
	Socks(const Socks&) = delete;
	Socks& operator=(const Socks&) = delete;
};

#endif // _SOCKS_H_
