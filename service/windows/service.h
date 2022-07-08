#include <windows.h>
#include <string>
#include <functional>


class ServiceWrapper {
 public:
  static std::function<void(std::string, bool)> startFunc;
  static void start(const std::string &_name);
private:
  static SERVICE_STATUS serviceStatus;
  static SERVICE_STATUS_HANDLE serviceStatusHandle;
  static HANDLE stopEvent;
  static SERVICE_TABLE_ENTRY serviceTable[2];
  static std::string serviceName;
  static void serviceMain(DWORD argc, LPTSTR *argv);
  static void serviceControlHandler(DWORD CtrlCode);
};
