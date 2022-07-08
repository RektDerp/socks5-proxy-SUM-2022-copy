#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>
#define init();
#define debug();
#include "debug.h"

#include <string.h>
#include <windows.h>
#include <exception>
#include <stdexcept>
#include <string>

#include "winnt.h"
#include "winsvc.h"

class ServiceWrapper{
public:
  static void start(const std::string &_name){
    debug("Constructor\n");
    serviceName = _name;
    serviceTable[0].lpServiceName = (LPSTR)_name.c_str();
    serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)serviceMain;
    if (StartServiceCtrlDispatcher (serviceTable) == false){
      throw(std::runtime_error("StartServiceCtrlDispatcher failed"));
    }
    serviceStatusHandle = RegisterServiceCtrlHandler(serviceName.c_str(),serviceControlHandler);
    if(!serviceStatusHandle)throw(std::runtime_error("RegisterServiceCtrlHandler failed"));
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    if(!SetServiceStatus(serviceStatusHandle, &serviceStatus))throw(std::runtime_error("SetServiceStatus failed"));

  }
private:
  static SERVICE_STATUS serviceStatus;
  static SERVICE_STATUS_HANDLE serviceStatusHandle;
  static HANDLE event;
  static SERVICE_TABLE_ENTRY serviceTable[2];
  static std::string serviceName;

  static void serviceMain (DWORD argc, LPTSTR *argv){
    debug("ServiceMain()\n");
    debug(serviceName.c_str());
  }
  static void serviceControlHandler (DWORD CtrlCode){
    debug("serviceControlHandler\n");
    switch (CtrlCode) {
      case SERVICE_CONTROL_STOP :
        break;
      default:
        break;
    }
  }
};


  SERVICE_STATUS ServiceWrapper::serviceStatus = {0};
  SERVICE_STATUS_HANDLE ServiceWrapper::serviceStatusHandle = NULL;
  HANDLE ServiceWrapper::event = INVALID_HANDLE_VALUE;
  SERVICE_TABLE_ENTRY ServiceWrapper::serviceTable[] = {0};
  std::string ServiceWrapper::serviceName;


int main(int argc, TCHAR *argv[]) {
  initdebug();

  debug("main() : startup\n");

  ServiceWrapper::start("Socks5");

  closesocket(debugsock);
  return 0;
}
