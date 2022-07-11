#ifndef _SOCKS5_IMPL_H_ 
#define _SOCKS5_IMPL_H_ 
#include "proxy_common.h"


// todo: move to config
// todo: store user/pass in db with password in encrypted
const bool AUTH_FLAG		= true;
const std::string USER		= "user";
const std::string PASSWORD	= "pass";

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
	ADDR_TYP_NOT_SUPP	= 0x08
};

class session;

class socks5_impl {
public:
	socks5_impl(session* s) : session_(s) {}
	~socks5_impl() = default;
	bool init();
	void write_stat(size_t bytes, bool isServer);
	void close();
private:
	socks5_impl(const socks5_impl&) = delete;

	bool auth();
	bool checkError(bs::error_code& ec);
	session* session_;
	std::string username_;
	long long id_ = 0;
};

#endif //_SOCKS5_IMPL_H_ 
