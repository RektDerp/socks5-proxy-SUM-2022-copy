//#define DEBUG
#include "debug.h"
#include "libloaderapi.h"
#include "service.h"
#include <exception>
#include <stdexcept>
#include <string>
#include <cstring>
#include <boost/function.hpp>

int main(int argc, char *argv[]) {
  char filename [256];
  char executable [] = "main.exe";
  initdebug();

  GetModuleFileName(NULL, filename, 256);

  *(strrchr(filename, '\\') + 1) = '\0';
  strcat(filename, executable);

  debug(filename);

  try {
    ServiceWrapper::executablePath = filename;
    ServiceWrapper::start("Socks5");
  } catch (std::runtime_error &e) {
    debug(e.what());
    debug("\n");
  }

  return 0;
}
