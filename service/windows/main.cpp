#include "libloaderapi.h"
#include "service.h"
#include <boost/function.hpp>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
extern FILE* logfile;

int main(int argc, char *argv[]) {
  char filename[256];
  char logfilepath[256];
  char executable[] = "main.exe";
  GetModuleFileName(NULL, filename, 256);
  *(strrchr(filename, '\\') + 1) = '\0';
  strcpy(logfilepath, filename);
  strcat(logfilepath, "log.txt");

  strcat(filename, executable);
  logfile = fopen(logfilepath, "w+");

  fprintf(logfile,"exec %s\nlog %s\n", filename, logfilepath);

  try {
    ServiceWrapper::executablePath = filename;
    ServiceWrapper::start("Socks5");
  } catch (std::runtime_error &e) {
    fprintf(logfile,"exception %s\n", e.what());
  }
  fclose(logfile);
  return 0;
}
