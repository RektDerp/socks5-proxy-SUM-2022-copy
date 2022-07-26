#ifndef _SOCKS5_IMPL_H_ 
#define _SOCKS5_IMPL_H_ 
#include "proxy_common.h"
#include "socks.h"
namespace proxy {
	const unsigned char AUTH_VER = 0x01;

	enum METHOD {
		NO_AUTH_REQ = 0x0,
		USERNAME_PASSWORD = 0x2,
		NO_ACC_METHODS = 0xFF
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

	class Socks5 : public Socks {
	public:
		Socks5(TcpSession* s);
		bool init() override;
	public:
		static bool AUTH_FLAG;

		inline static METHOD getServerMethod()
		{
			return AUTH_FLAG ? USERNAME_PASSWORD : NO_AUTH_REQ;
		}
	private:
		unsigned char _atyp;
		unsigned char _cmd;
		

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
	};
} // namespace proxy
#endif //_SOCKS5_IMPL_H_ 
