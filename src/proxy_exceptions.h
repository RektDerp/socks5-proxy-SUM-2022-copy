#ifndef _PROXY_EXCEPTIONS_H_
#define _PROXY_EXCEPTIONS_H_

#include <string>
#include <exception>
#include <stdexcept>
namespace proxy {
	class ProxyException : public std::runtime_error {
	public:
		ProxyException() = default;
		ProxyException(const std::string& msg) : std::runtime_error(msg)
		{}
		virtual ~ProxyException() = default;
	};

	class DatabaseException : public ProxyException {
	public:
		DatabaseException() = default;
		DatabaseException(const std::string& msg) : ProxyException(msg.c_str())
		{}
		virtual ~DatabaseException() = default;
	};
} // namespace proxy
#endif // _PROXY_EXCEPTIONS_H_
