#include "libloaderapi.h"
#include "service.h"
#include <boost/function.hpp>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
extern FILE* log;

int main(int argc, char *argv[]) {
  char filename[256];
  char executable[] = "main.exe";
  log = fopen("C:\\log.txt", "w+");
  GetModuleFileName(NULL, filename, 256);

  *(strrchr(filename, '\\') + 1) = '\0';
  strcat(filename, executable);
  fprintf(log,"exec %s\n", filename);

  try {
    ServiceWrapper::executablePath = filename;
    ServiceWrapper::start("Socks5");
  } catch (std::runtime_error &e) {
    fprintf(log,"exception %s\n", e.what());
  }

  return 0;
}
