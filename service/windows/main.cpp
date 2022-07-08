#include "service.h"
#include <exception>
#include <stdexcept>
#include <boost/function.hpp>

int main(int argc, char *argv[]) {
  try {
    ServiceWrapper::startFunc = [](std::string a, bool action) {

    };
    ServiceWrapper::start("Socks5");
  } catch (std::runtime_error &e) {

  }

  return 0;
}
