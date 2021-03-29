#pragma once

#include "status.h"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace cp1craft {
namespace io {

bool CreateSocket(int socket_family, int socket_type, int protocol, int &sockfd,
                  status &s);
bool CreateTcpSocketandConnect(const std::string &ip, int port,
                               int conn_timeout, bool nonblock, int &fd,
                               status &s);
bool CreateTcpSocketforListen(bool reuse, int &fd, status &s);
bool CreateTcpSocketandBindPort(int port_start, int max_retry, int interval, int &sockfd, int &port, status &s);

bool SetSocketNonblock(int fd, bool nonblock, status &s);
bool PollSocket(int fd, int timeout /*in ms*/, struct addrinfo *info,
                status &s);

struct TcpAddrInfo {
  TcpAddrInfo(const std::string &ip, int port, status &s);
  ~TcpAddrInfo();

  bool OK() { return sockfd > 0; }

  struct addrinfo *info;
  int sockfd;
};

} // namespace io
} // namespace cp1craft
