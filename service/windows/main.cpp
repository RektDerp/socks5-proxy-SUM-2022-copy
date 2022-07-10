#include "debug.h"
#include "service.h"
#include <exception>
#include <stdexcept>
#include <string>
#include <boost/function.hpp>

int main(int argc, char *argv[]) {
  initdebug();
  try {
    ServiceWrapper::executablePath = "Z:\\home\\derpy\\git\\socks5-proxy-SUM-2022\\bin\\main.exe";
    ServiceWrapper::start("Socks5");
  } catch (std::runtime_error &e) {
    debug(e.what());
  }

  return 0;
}
