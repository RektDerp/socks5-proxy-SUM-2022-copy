#ifndef _PROXY_COMMON_H
#define _PROXY_COMMON_H

#include <iostream>
#include <thread>
#include <bitset>
#include <string>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace ba = boost::asio;
namespace bs = boost::system;
//const ba::ip::tcp ipv4 = ba::ip::tcp::v4();

const size_t BUFFER_LEN = 8192;
const unsigned short listen_port = 1080;

#endif // _PROXY_COMMON_H
