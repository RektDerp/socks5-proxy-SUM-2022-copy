#include "libloaderapi.h"
#include "service.h"
#include <boost/function.hpp>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
extern FILE* logfile;

int main(int argc, char *argv[]) {
  wchar_t filename[256];
  wchar_t logfilepath[256];
  wchar_t executable[] = L"main.exe";
  wchar_t logfilename [] = L"service.log";
  GetModuleFileNameW(NULL, filename, 256);
  *(wcsrchr(filename, L'\\') + 1) = '\0';
  wcscpy(logfilepath, filename);
  wcscat(logfilepath, logfilename);

  wcscat(filename, executable);
  logfile = _wfopen(logfilepath, L"w+");

  fwprintf(logfile,L"exec %s\nlog %s\n", filename, logfilepath);

  try {
    ServiceWrapper::executablePath = filename;
    ServiceWrapper::start(L"Socks5");
  } catch (std::runtime_error &e) {
    fprintf(logfile,"exception %s\n", e.what());
  }
  fclose(logfile);
  return 0;
}
