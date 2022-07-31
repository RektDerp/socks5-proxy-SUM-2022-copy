#include <windows.h>
#include <string>
#include <functional>
#define BUFSIZE 1

class ServiceWrapper {
 public:
  static std::string executablePath;
  static void start(const std::string &_name);
private:
  static SERVICE_STATUS serviceStatus;
  static SERVICE_STATUS_HANDLE serviceStatusHandle;
  static HANDLE stopEvent;
  static SERVICE_TABLE_ENTRY serviceTable[2];
  static STARTUPINFO serviceStartupInfo;
  static PROCESS_INFORMATION serviceProcessInfo;
  static SECURITY_ATTRIBUTES pipeSecurityAttributes;
  static std::string serviceName;
  static void serviceMain(DWORD argc, LPTSTR *argv);
  static void serviceControlHandler(DWORD CtrlCode);
};
