#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include "proxy_common.h"
#include <sstream>

namespace string_utils {
	using std::string;

	inline string to_string(const bvec& vec)
	{
		std::stringstream ss;
		for (auto c : vec)
			ss << c;
		return ss.str();
	}

	inline string formIpAddressString(const bvec& bytes)
	{
		std::string ip_address;
		for (int i = 0; i < 4; i++)
		{
			ip_address += std::to_string((int) bytes[i]);
			if (i != 3) ip_address += '.';
		}
		return ip_address;
	}
}

#endif
