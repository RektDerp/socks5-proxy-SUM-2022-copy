#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <wchar.h>
#include <windows.h>
#include <string>
#include <functional>
#define BUFSIZE 1

class ServiceWrapper {
 public:
  static LPWSTR executablePath;
  static void start(const LPWSTR &_name);
private:
  static SERVICE_STATUS serviceStatus;
  static SERVICE_STATUS_HANDLE serviceStatusHandle;
  static HANDLE stopEvent;
  static SERVICE_TABLE_ENTRY serviceTable[2];
  static STARTUPINFO serviceStartupInfo;
  static PROCESS_INFORMATION serviceProcessInfo;
  static SECURITY_ATTRIBUTES pipeSecurityAttributes;
  static LPWSTR serviceName;
  static void serviceMain(DWORD argc, LPTSTR *argv);
  static void serviceControlHandler(DWORD CtrlCode);
};
