#ifndef _PROXY_EXCEPTIONS_H_
#define _PROXY_EXCEPTIONS_H_

#include <string>
#include <exception>
#include <stdexcept>

class proxy_exception : public std::runtime_error {
public:
	proxy_exception() = default;
	proxy_exception(const std::string& msg) : std::runtime_error(msg)
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
