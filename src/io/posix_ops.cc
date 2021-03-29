#include "posix_ops.h"

#include <errno.h>
#include <fmt/core.h>
#include <sys/socket.h>

namespace cp1craft {
namespace io {

#define SysCallErrRecoder(syscall, s)                                          \
  do {                                                                         \
    s << "call " #syscall " failed: " << strerror(errno);                      \
  } while (0)

bool CreateSocket(int socket_family, int socket_type, int protocol, int &sockfd,
                  status &s) {
  sockfd = socket(socket_family, socket_type, protocol);
  if (sockfd < 0) {
    SysCallErrRecoder(socket, s);
    return false;
  }
  return true;
}

//
// create a socket connected to a @ip:@port within @timeout ms.
//
bool CreateTcpSocketforConnect(const std::string &ip, int port,
                               int conn_timeout, bool nonblock, int &fd,
                               status &s) {
  TcpAddrInfo addr(ip, port, s);
  if (!addr.OK()) {
    goto ERR_ADDRHINT;
  }
  fd = addr.sockfd;
  if (!SetSocketNonblock(fd, true, s)) {
    goto ERR_SETNONBLOCK;
  }


ERR_SETNONBLOCK:
  if (fd > 0) {
    close(fd);
    fd = -1;
  }
ERR_ADDRHINT:
  return false;
}
bool CreateSocketforListen(bool reuse, int &fd);
bool BindPort(int fd, int port_start, int max_retry, int &port);

bool SetSocketNonblock(int fd, bool nonblock, status &s) {
  int flags;
  flags = fcntl(fd, F_GETFL, NULL);
  if (flags < 0) {
    SysCallErrRecoder(fcntl, s);
    s << ",fd=" << fd;
    return false;
  }
  if (nonblock) {
    flags = flags | O_NONBLOCK; // add non-block
  } else {
    flags = flags & (~O_NONBLOCK); // remove non-block
  }
  if (fcntl(fd, F_SETFL, flags) == -1) {
    SysCallErrRecoder(fcntl, s);
    s << ",fd=" << fd << ",flags=" << flags;
    return false;
  }
  return true;
}

TcpAddrInfo::TcpAddrInfo(const std::string &ip, int port, status &s)
    : info(nullptr), socka(nullptr), socka_len(0), sockfd(-1) {
  struct addrinfo hints;
  char _port[8];
  snprintf(_port, 8, "%d", port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // addrinfo
  int r = getaddrinfo(ip.c_str(), _port, &hints, &info);
  if (r != 0) {
    s << "call getaddrinfo failed: " << gai_strerror(r);
    info = nullptr;
    return;
  }

  // sockaddr
  socka = (struct sockaddr *)malloc(info->ai_addrlen);
  if (socka == nullptr) {
    SysCallErrRecoder(malloc, s);
    return;
  }
  memcpy(socka, info->ai_addr, info->ai_addrlen);
  socka_len = info->ai_addrlen;

  if (!CreateSocket(info->ai_family, info->ai_socktype, info->ai_protocol,
                    sockfd, s)) {
    sockfd = -1;
    return;
  }
}

TcpAddrInfo::~TcpAddrInfo() {
  if (info) {
    freeaddrinfo(info);
  }
  if (socka) {
    free(socka);
  }
}

} // namespace io
} // namespace cp1craft
