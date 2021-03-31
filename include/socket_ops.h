#pragma once

#include "status.h"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace cp1craft {
namespace io {

using status = cp1craft::utils::status;
bool CreateSocket(int socket_family, int socket_type, int protocol, int &sockfd,
                  status &s);
bool CreateTcpSocketandConnect(const std::string &ip, int port,
                               int conn_timeout, bool nonblock, int &fd,
                               status &s);
bool CreateTcpSocketforListenNonblock(bool reuse, int &fd, status &s);
bool CreateTcpSocketandBindUsablePort(int port_start, int max_retry,
                                      int interval, int &fd, int &port,
                                      status &s);
bool BindSocketWithUsablePort(int fd, int port_start, int port_max,
                              int interval, int &rport, status &s);
bool SetSocketNonblock(int fd, bool nonblock, status &s);
bool PollSocket(int fd, int timeout /*in ms*/, status &s);

struct TcpAddrInfo {
  TcpAddrInfo(const std::string &ip, int port, status &s);
  ~TcpAddrInfo();
  bool OK();

  struct addrinfo *info;
};

} // namespace io
} // namespace cp1craft
