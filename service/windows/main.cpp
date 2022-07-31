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
  char logfile[256];
  char executable[] = "main.exe";
  GetModuleFileName(NULL, filename, 256);
  *(strrchr(filename, '\\') + 1) = '\0';
  strcpy(logfile, filename);
  strcat(logfile, "log.txt");

  strcat(filename, executable);
  log = fopen(logfile, "w+");

  fprintf(log,"exec %s\nlog %s\n", filename, logfile);

  try {
    ServiceWrapper::executablePath = filename;
    ServiceWrapper::start("Socks5");
  } catch (std::runtime_error &e) {
    fprintf(log,"exception %s\n", e.what());
  }
  fclose(log);
  return 0;
}
