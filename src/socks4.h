#ifndef _SOCKS4_H_
#define _SOCKS4_H_
#include "socks.h"


enum REPLY {
	GRANTED = 90,
	REJECTED = 91
};

class socks4 : public socks {
public:
	socks4(session* const s): socks(s, SOCKS4_VER)
	{}
	~socks4() = default;
	bool init();
};

#endif // _SOCKS4_H_
