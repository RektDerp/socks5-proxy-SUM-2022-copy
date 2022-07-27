#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <sstream>
#include <vector>
#include <algorithm>

using bvec = std::vector<unsigned char>;

namespace proxy { namespace string_utils {
	using std::string;

	inline string to_string(const bvec& vec)
	{
		std::stringstream ss;
		for (auto c : vec)
			ss << c;
		return ss.str();
	}

	inline string to_string(const unsigned char* c)
	{
		if (c == nullptr) {
			return string("");
		}
		return string((const char*) c);
	}

	inline string formIpAddressString(const bvec& bytes)
	{
		std::string ip_address;
		if (bytes.size() == 4)
		{
			for (int i = 0; i < 4; i++)
			{
				ip_address += std::to_string((int)bytes[i]);
				if (i != 3) ip_address += '.';
			}
		}
		else
		{
			for (int i = 0; i < 16; i++)
			{
				std::stringstream ss;
				ss << std::hex << (int)bytes[i];
				i++;
				ss << std::hex << (int)bytes[i];
				ip_address += ss.str();
				if (i != 15) ip_address += ':';
			}
		}
		return ip_address;
	}

	inline string formIpAddressReverseString(const bvec& bytes)
	{
		std::string ip_address;
		for (size_t i = bytes.size() - 1; i >= 0; i--)
		{
			ip_address += std::to_string((int)bytes[i]);
			if (i != 0) ip_address += '.';
		}
		return ip_address;
	}

	template<class T>
	inline string concat(const char* s, T i)
	{
		std::stringstream ss;
		ss << s << i;
		return ss.str();
	}
}} // namespace proxy string_utils

#endif // _STRING_UTILS_H_
