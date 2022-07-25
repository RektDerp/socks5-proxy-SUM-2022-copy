#ifndef _SOCKS4_H_
#define _SOCKS4_H_
#include "socks.h"


enum REPLY {
	GRANTED = 90,
	REJECTED = 91
};

class Socks4 : public Socks {
public:
	Socks4(TcpSession* const session): Socks(session, SOCKS4_VER)
	{}
	~Socks4() = default;
	bool init() override;
};

#endif // _SOCKS4_H_
