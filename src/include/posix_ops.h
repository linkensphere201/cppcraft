#pragma once

#include "status.h"

namespace cp1craft {
namespace io {

bool CreateSocket(int socket_family, int socket_type, int protocol,
                  int &sockfd);
bool CreateTcpSocketforConnect(const std::string &ip, int port,
                               int conn_timeout, bool nonblock, int &fd,
                               status &s);
bool CreateTcpSocketforListen(bool reuse, int &fd, status &s);
bool BindPort(int fd, int port_start, int max_retry, int &port);
bool SetSocketNonblock(int fd, bool nonblock, status &s);

struct TcpAddrInfo {
  TcpAddrInfo(const std::string &ip, int port, status &s);
  ~TcpAddrInfo();

  bool OK() { return sockfd > 0; }

  struct addrinfo *info;
  struct sockaddr *socka;
  size_t socka_len;
  int sockfd;
};

} // namespace io
} // namespace cp1craft
