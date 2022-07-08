#include "service.h"
#include <boost/function.hpp>
#include "synchapi.h"
#include <exception>
#include <stdexcept>
#include "winnt.h"
#include "winsvc.h"

SERVICE_STATUS ServiceWrapper::serviceStatus = {0};
SERVICE_STATUS_HANDLE ServiceWrapper::serviceStatusHandle = NULL;
HANDLE ServiceWrapper::stopEvent = 0;
SERVICE_TABLE_ENTRY ServiceWrapper::serviceTable[] = {0};
std::string ServiceWrapper::serviceName;
std::function<void(std::string, bool)> ServiceWrapper::startFunc;

void ServiceWrapper::start(const std::string &_name) {

    serviceName = _name;
    serviceTable[0].lpServiceName = (LPSTR)_name.c_str();
    serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)serviceMain;
    if (StartServiceCtrlDispatcher(serviceTable) == false) {
      throw(std::runtime_error("StartServiceCtrlDispatcher failed"));
    }
  }

void ServiceWrapper::serviceMain(DWORD argc, LPTSTR *argv) {
    serviceStatusHandle =
        RegisterServiceCtrlHandler(serviceName.c_str(), serviceControlHandler);
    if (!serviceStatusHandle)
      throw(std::runtime_error("RegisterServiceCtrlHandler failed"));
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
      throw(std::runtime_error("SetServiceStatus failed"));

    stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!stopEvent) {
      serviceStatus.dwCheckPoint = 1;
      serviceStatus.dwCurrentState = SERVICE_STOPPED;
      serviceStatus.dwWin32ExitCode = GetLastError();
      if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
        throw(std::runtime_error("SetServiceStatus failed"));
      return;
    }

    startFunc(serviceName, true);

    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
      throw(std::runtime_error("SetServiceStatus failed"));

    while (true) {
      WaitForSingleObject(stopEvent, INFINITE);
      startFunc(serviceName, false);
      serviceStatus.dwCurrentState = SERVICE_STOPPED;
      if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
        throw(std::runtime_error("SetServiceStatus failed"));
      return;
    }
  }

void ServiceWrapper::serviceControlHandler(DWORD CtrlCode) {

    switch (CtrlCode) {
      case SERVICE_CONTROL_STOP:
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
          throw(std::runtime_error("SetServiceStatus failed"));
        SetEvent(stopEvent);
        break;
      default:
        break;
    }
  }

