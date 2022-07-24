#ifndef _SOCKS_H_
#define _SOCKS_H_
#include "proxy_common.h"

#ifdef STAT
#include "stat_db_service.h"
#endif
const unsigned char RSV = 0x00;

enum VER {
	SOCKS4_VER = 4,
	SOCKS5_VER = 5
};

enum METHOD {
	NO_AUTH_REQ = 0x0,
	USERNAME_PASSWORD = 0x2,
	NO_ACC_METHODS = 0xFF
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

enum ATYP {
	IPV4 = 0x1,
	DOMAIN_NAME = 0x3,
	IPV6 = 0x4
};

enum REP {
	SUCCEEDED = 0x00,
	SERVER_FAILURE = 0x01,
	CONN_NOT_ALLOWED = 0x02,
	NETWORK_UNREACH = 0x03,
	HOST_UNREACHABLE = 0x04,
	CONNECTION_REFUSED = 0x05,
	TTL_EXPIRED = 0x06,
	COMMAND_NOT_SUPP = 0x07,
	ADDR_TYPE_NOT_SUPP = 0x08
};

class TcpSession;

class Socks {
protected:
	Socks(TcpSession* const s, VER socks_ver):
		_session(s), _id(0), _socks_ver(socks_ver)
	{}
public:
	//virtual ~Socks() = 0;
	virtual bool init() = 0;
	void close();
	void write_stat(size_t bytes, bool isServer);
protected:
	TcpSession* const _session;
	long long _id;
	const VER _socks_ver;
	unsigned short _serverPort;
	unsigned short _bindPort;
	std::string _dstAddress;
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
