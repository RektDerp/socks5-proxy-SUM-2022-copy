#ifndef _PROXY_COMMON_H
#define _PROXY_COMMON_H

#define STAT
//#define STAT_CSV

#include <iostream>
#include <thread>
#include <bitset>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <memory>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace ba = boost::asio;
namespace bs = boost::system;

using bvec = std::vector<unsigned char>;

#endif // _PROXY_COMMON_H
