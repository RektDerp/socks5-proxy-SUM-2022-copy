#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#include <exception>
#include <stdexcept>
#include <string>

#include "winnt.h"
#include "winsvc.h"

#define debug(x){send(debugsock, (x), strlen(x), 0);}
inline unsigned int getIPAddress(char a, char b, char c, char d) {
  return (((a & 0xff) << 24) | ((b & 0xff) << 16) | ((c & 0xff) << 8) |
          (d & 0xff));
}
const int IP = getIPAddress(127, 0, 0, 1);
const int PORT = 8080;
const char SOCKOPTION = 1;
struct sockaddr_in servaddr;
SOCKET debugsock;

int main(int argc, TCHAR *argv[]) {
  WSADATA wsaData = {0};
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  debugsock = socket(AF_INET, SOCK_STREAM, 0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(IP);
  servaddr.sin_port = htons(PORT);
  setsockopt(debugsock, SOL_SOCKET, SO_REUSEADDR, &SOCKOPTION,
             sizeof(SOCKOPTION));
  if (connect(debugsock, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    printf("pizdiec\n");
    return -1;
  }

  debug("main() : startup\n");



  closesocket(debugsock);
  return 0;
}
