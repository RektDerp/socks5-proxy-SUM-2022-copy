#ifndef _SOCKS5_IMPL_H_ 
#define _SOCKS5_IMPL_H_ 
#include "proxy_common.h"
#include "LogConfigReader.h"

const unsigned char SOCKS_VER	= 0x05;
const unsigned char AUTH_VER	= 0x01;
const unsigned char RSV			= 0x00;

enum METHOD {
	NO_AUTH_REQ			= 0x0,
	USERNAME_PASSWORD	= 0x2,
	NO_ACC_METHODS		= 0xFF
};

enum AUTH_STATUS {
	SUCCESS = 0x0,
	DENY	= 0x1
};

enum CMD {
	CONNECT			= 0x1,
	BIND			= 0x2,
	UDP_ASSOCIATE	= 0x3
};

enum ATYP {
	IPV4		= 0x1,
	DOMAIN_NAME = 0x3,
	IPV6		= 0x4
};

enum REP {
	SUCCEEDED			= 0x00,
	SERVER_FAILURE		= 0x01,
	CONN_NOT_ALLOWED	= 0x02,
	NETWORK_UNREACH		= 0x03,
	HOST_UNREACHABLE	= 0x04,
	CONNECTION_REFUSED	= 0x05,
	TTL_EXPIRED			= 0x06,
	COMMAND_NOT_SUPP	= 0x07,
	ADDR_TYPE_NOT_SUPP	= 0x08
};

class session;

class socks5_impl {
public:
	socks5_impl(session* s) : _session(s), _authFlag(false)
	{
		LogConfigReader::getInstance()->getValue("Auth", _authFlag);
	}
	~socks5_impl() = default;
	bool init();
	void write_stat(size_t bytes, bool isServer);
	void close();
private:
	session* _session;
	std::string _username;
	long long id_ = 0;
	unsigned char _atyp;
	unsigned char _cmd;
	std::string _dstAddress;
	unsigned short _serverPort;
	unsigned short _bindPort;
	bool _authFlag;

	socks5_impl(const socks5_impl&) = delete;
	socks5_impl& operator=(const socks5_impl&) = delete;

	bool checkVersion();
	bool checkMethod();
	bool auth();
	bool checkError(bs::error_code& ec);
	void sendErrorResponse(REP responseCode);
	std::string readAddress(unsigned char atyp);
	bool readMethodRequest();
	bool sendMethodResponse(METHOD method);
	bool readCommandRequest();
	bool sendCommandResponse(unsigned short bindPort);
	bool createRecord();
	bool connect(ba::ip::tcp::resolver::query query);

	inline METHOD getServerMethod()
	{
		return _authFlag ? USERNAME_PASSWORD : NO_AUTH_REQ;
	}
};

#endif //_SOCKS5_IMPL_H_ 
