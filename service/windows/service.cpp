#include "service.h"
#include "namedpipeapi.h"
#include "processthreadsapi.h"
#include "synchapi.h"
#include "winnt.h"
#include "winsvc.h"
#include <boost/function.hpp>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <string>

SERVICE_STATUS ServiceWrapper::serviceStatus = {0};
SERVICE_STATUS_HANDLE ServiceWrapper::serviceStatusHandle = NULL;
HANDLE ServiceWrapper::stopEvent = 0;
SERVICE_TABLE_ENTRY ServiceWrapper::serviceTable[] = {0};
STARTUPINFO ServiceWrapper::serviceStartupInfo = {0};
PROCESS_INFORMATION ServiceWrapper::serviceProcessInfo = {0};
SECURITY_ATTRIBUTES ServiceWrapper::pipeSecurityAttributes = {0};
std::string ServiceWrapper::serviceName;
std::string ServiceWrapper::executablePath;
FILE* log;

void ServiceWrapper::start(const std::string &_name) {

  serviceName = _name;
  serviceTable[0].lpServiceName = (LPSTR)_name.c_str();
  serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)serviceMain;
  if (StartServiceCtrlDispatcher(serviceTable) == false) {
    throw(std::runtime_error("StartServiceCtrlDispatcher failed"));
  }
}

void ServiceWrapper::serviceMain(DWORD argc, LPTSTR *argv) {

  serviceStatusHandle = RegisterServiceCtrlHandler(serviceName.c_str(), serviceControlHandler);
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
  fprintf(log, "Starting server\n");
  pipeSecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  pipeSecurityAttributes.bInheritHandle = TRUE;
  pipeSecurityAttributes.lpSecurityDescriptor = NULL;
  HANDLE pipeStdoutWr = NULL;
  HANDLE pipeStdoutRd = NULL;
  if(!CreatePipe(&pipeStdoutRd, &pipeStdoutWr, &pipeSecurityAttributes, 0))
    throw(std::runtime_error("CreatePipe failed"));


  serviceStartupInfo.cb = sizeof(serviceStartupInfo);
  serviceStartupInfo.hStdOutput = pipeStdoutWr;
  serviceStartupInfo.hStdError = pipeStdoutWr;
  serviceStartupInfo.dwFlags = STARTF_USESTDHANDLES;
  if (!CreateProcess(NULL, (LPSTR)executablePath.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &serviceStartupInfo, &serviceProcessInfo)) {
    std::string errortext = "Failed to create process: ";
    errortext.append(std::to_string(GetLastError()));
    throw(std::runtime_error(errortext));
  }

  /////////////

  serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  serviceStatus.dwCurrentState = SERVICE_RUNNING;
  if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
    throw(std::runtime_error("SetServiceStatus failed"));
  fprintf(log, "Service started, logging data from server process\n");
  char buffer[BUFSIZE + 1] = {0};
  BOOL status;
  DWORD nbytes;

  while (true) {

    if (WaitForSingleObject(stopEvent, 100) != WAIT_TIMEOUT) {
      // stop process
      fprintf(log, "Stopping server\n");
      TerminateProcess(serviceProcessInfo.hProcess, 0);
      CloseHandle(serviceProcessInfo.hProcess);
      CloseHandle(serviceProcessInfo.hThread);
      serviceStatus.dwCurrentState = SERVICE_STOPPED;
      serviceStatus.dwWin32ExitCode = 0;
      if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
        throw(std::runtime_error("SetServiceStatus failed"));
      return;
    }

    if (WaitForSingleObject(serviceProcessInfo.hProcess, 100) !=
        WAIT_TIMEOUT) {
      fprintf(log, "Server process died\n");
      CloseHandle(serviceProcessInfo.hProcess);
      CloseHandle(serviceProcessInfo.hThread);
      serviceStatus.dwCurrentState = SERVICE_STOPPED;
      serviceStatus.dwWin32ExitCode = -1;
      if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
        throw(std::runtime_error("SetServiceStatus failed"));
      return;
    }
    PeekNamedPipe(pipeStdoutRd, buffer, BUFSIZE, NULL, &nbytes, NULL);
    if(nbytes > 0){
        status = ReadFile(pipeStdoutRd, buffer, BUFSIZE, &nbytes, NULL);
        if(!status)
          throw(std::runtime_error("Read from pipe failed"));
        fwrite(buffer,BUFSIZE,1,log);
    }
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
