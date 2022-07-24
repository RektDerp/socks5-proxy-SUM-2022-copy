#ifndef _SOCKS5_IMPL_H_ 
#define _SOCKS5_IMPL_H_ 
#include "proxy_common.h"
#include "socks.h"

const unsigned char AUTH_VER = 0x01;

class Socks5 : public Socks {
public:
	Socks5(TcpSession* s);
	~Socks5() = default;
	bool init() override;
private:
	unsigned char _atyp;
	unsigned char _cmd;
	bool _authFlag;

	Socks5(const Socks5&) = delete;
	Socks5& operator=(const Socks5&) = delete;

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
