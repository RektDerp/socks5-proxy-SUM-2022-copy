#ifndef _SOCKS5_IMPL_H_ 
#define _SOCKS5_IMPL_H_ 
#include "proxy_common.h"
#include "socks.h"

const unsigned char AUTH_VER = 0x01;

class socks5_impl : public socks {
public:
	socks5_impl(session* s);
	~socks5_impl() = default;
	bool init() override;
private:
	unsigned char _atyp;
	unsigned char _cmd;
	bool _authFlag;

	socks5_impl(const socks5_impl&) = delete;
	socks5_impl& operator=(const socks5_impl&) = delete;

	bool checkMethod();
	bool auth();
	void sendErrorResponse(REP responseCode);
	std::string readAddress(unsigned char atyp);
	bool readMethodRequest();
	bool sendMethodResponse(METHOD method);
	bool readCommandRequest();
	bool sendCommandResponse(unsigned short bindPort);
	bool connect(ba::ip::tcp::resolver::query query);

	inline METHOD getServerMethod()
	{
		return _authFlag ? USERNAME_PASSWORD : NO_AUTH_REQ;
	}
};

#endif //_SOCKS5_IMPL_H_ 
