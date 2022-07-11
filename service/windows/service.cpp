//#define DEBUG_AUX
#include "debug.h"
#include "service.h"
#include <boost/function.hpp>
#include "processthreadsapi.h"
#include "synchapi.h"
#include <exception>
#include <stdexcept>
#include <string>
#include "winnt.h"
#include "winsvc.h"

SERVICE_STATUS ServiceWrapper::serviceStatus = {0};
SERVICE_STATUS_HANDLE ServiceWrapper::serviceStatusHandle = NULL;
HANDLE ServiceWrapper::stopEvent = 0;
SERVICE_TABLE_ENTRY ServiceWrapper::serviceTable[] = {0};
STARTUPINFO ServiceWrapper::serviceStartupInfo = {0};
PROCESS_INFORMATION ServiceWrapper::serviceProcessInfo = {0};
std::string ServiceWrapper::serviceName;
std::string ServiceWrapper::executablePath;

void ServiceWrapper::start(const std::string &_name) {

    debug("\nstart()\n");
    serviceName = _name;
    serviceTable[0].lpServiceName = (LPSTR)_name.c_str();
    serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)serviceMain;
    if (StartServiceCtrlDispatcher(serviceTable) == false) {
      throw(std::runtime_error("StartServiceCtrlDispatcher failed"));
    }
  }

void ServiceWrapper::serviceMain(DWORD argc, LPTSTR *argv) {
  debug("serviceMain()\n");
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

    /////////////
    debug("starting server\n");
    serviceStartupInfo.cb = sizeof(serviceStartupInfo);
    if(!CreateProcess(
      NULL,
      (LPSTR)executablePath.c_str(),
      NULL,
      NULL,
      FALSE,
      0,
      NULL,
      NULL,
      &serviceStartupInfo,
      &serviceProcessInfo
    )){
      std::string errortext = "Failed to create process: ";
      errortext.append(std::to_string(GetLastError()));
      throw(std::runtime_error(errortext));
    }

    /////////////

    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
      throw(std::runtime_error("SetServiceStatus failed"));

    while (true) {
      debug("loop:\n");
      if(WaitForSingleObject(stopEvent,1000) != WAIT_TIMEOUT){
        //stop process
        debug("service stop event received\n");
        TerminateProcess(serviceProcessInfo.hProcess, 0);
        CloseHandle(serviceProcessInfo.hProcess);
        CloseHandle(serviceProcessInfo.hThread);
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = 0;
        if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
          throw(std::runtime_error("SetServiceStatus failed"));
        return;
      }
      debug("wait for stop event not recieved\n");
      if(WaitForSingleObject(serviceProcessInfo.hProcess, 1000) != WAIT_TIMEOUT){
        debug("server process died\n");
        CloseHandle(serviceProcessInfo.hProcess);
        CloseHandle(serviceProcessInfo.hThread);
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = -1;
        if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
          throw(std::runtime_error("SetServiceStatus failed"));
        return;
      }
      debug("process is alive\n");
    }
  }

void ServiceWrapper::serviceControlHandler(DWORD CtrlCode) {

    switch (CtrlCode) {
      case SERVICE_CONTROL_STOP:
        debug("got stop control\n");
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
          throw(std::runtime_error("SetServiceStatus failed"));
        SetEvent(stopEvent);
        break;
      default:
        break;
    }
  }

