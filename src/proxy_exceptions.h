#ifndef _PROXY_EXCEPTIONS_H_
#define _PROXY_EXCEPTIONS_H_

#include <string>

class proxy_exception : public std::exception {
public:
	proxy_exception() = default;
	proxy_exception(const std::string& msg) : std::exception(msg.c_str())
	{}
	virtual ~proxy_exception() = default;
};

class db_exception : public proxy_exception {
public:
	db_exception() = default;
	db_exception(const std::string& msg) : proxy_exception(msg.c_str())
	{}
	virtual ~db_exception() = default;
};

#endif
