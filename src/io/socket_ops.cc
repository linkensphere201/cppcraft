#include "socket_ops.h"

#include <fmt/core.h>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

// TODO - to make this portable

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
bool CreateTcpSocketandConnect(const std::string &ip, int port,
                               int conn_timeout, bool nonblock, int &fd,
                               status &s) {
  TcpAddrInfo addr(ip, port, s);
  int         sockfd = -1;
  int         ret;
  if (!addr.OK()) {
    goto ERR_ADDRHINT;
  }
  if (!CreateSocket(addr.info->ai_family, addr.info->ai_socktype,
                    addr.info->ai_protocol, sockfd, s)) {
    sockfd = -1;
    goto ERR_CREATSOCKET;
  }

  if (!SetSocketNonblock(sockfd, true, s)) {
    goto ERR_SETNONBLOCK;
  }

  ret = connect(sockfd, addr.info->ai_addr, addr.info->ai_addrlen);
  if (ret == -1) { // some error so need to poll
    if (errno == EINPROGRESS) {
      if (!PollSocket(sockfd, conn_timeout, s)) {
        goto ERR_CONN;
      } else {
        ret = connect(sockfd, addr.info->ai_addr, addr.info->ai_addrlen);
        if (ret != 0 && errno != EISCONN) {
          if (errno == EALREADY || errno == EINPROGRESS || errno == EWOULDBLOCK) {
            s << "connection timeout: " << strerror(ETIMEDOUT);
          } else {
            s << "connection failed: " << strerror(errno);
          }
          goto ERR_CONN;
        }
      }
    } else {
      SysCallErrRecoder(connect, s);
      goto ERR_CONN;
    }
  }

  if (!nonblock && !SetSocketNonblock(sockfd, false, s)) {
    goto ERR_SETBLOCK;
  }

  fd = sockfd;
  return true;

ERR_SETBLOCK:
ERR_CONN:
ERR_SETNONBLOCK:
  if (sockfd > 0) {
    close(sockfd);
    sockfd = -1;
  }
ERR_CREATSOCKET:
ERR_ADDRHINT:
  return false;
}

bool CreateSocketforListenNonblock(bool reuse, int &fd, status &s) {
  int rc;
  int sockfd = -1;
  int              flags = 1;
  std::vector<int> optnames;
  
  if (!CreateSocket(AF_INET, SOCK_STREAM, 0, sockfd, s)) {
    goto ERR_SOCK;
  }
  if (!SetSocketNonblock(sockfd, true, s)) {
    goto ERR_SETNONBLOCK;
  }
  optnames.push_back(SO_KEEPALIVE);

#if defined(SO_REUSEADDR)
  optnames.push_back(SO_REUSEADDR);
#endif

#if defined(SO_REUSEPORT)
  optnames.push_back(SO_REUSEPORT);
#endif

  for (auto sopt : optnames) {
    if (sopt == SO_REUSEPORT && !reuse) {
      continue;
    }
    rc = setsockopt(sockfd, SOL_SOCKET, sopt, (void *)&flags,
                    (socklen_t)sizeof(flags));
    if (rc != 0) {
      SysCallErrRecoder(setsockopt, s);
      s << ", opt=" << sopt << ", flags=" << flags;
      goto ERR_SETSOCKOPT;
    }
  }

  fd = sockfd;
  return true;

ERR_SETSOCKOPT:
ERR_SETNONBLOCK:
  if (sockfd > 0) {
    close(sockfd);
    sockfd = -1;
  }
ERR_SOCK:
  return false;
}

bool BindSocketWithUsablePort(int fd, int port_start, int port_max,
                              int interval/*usec*/, int &rport, status &s) {
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  bool ret            = false;

  for (int port = port_start; port < port_max; port++) {
    sin.sin_port = htons(port);
    int rc       = bind(fd, (struct sockaddr *)&sin, sizeof(sin));
    if (0 == rc) {
      rport = port;
      ret   = true;
      break;
    } else if (errno != EADDRINUSE) {
      SysCallErrRecoder(bind, s);
      s << ",fd=" << fd;
      break;
    }
    usleep(interval);
  }
  return ret;
}

bool CreateTcpSocketandBindUsablePort(int port_start, int max_retry,
                                      int interval, int &fd, int &port,
                                      status &s) {
  int sockfd = -1, sockport = -1;
  if (!CreateTcpSocketforListenNonblock(false, sockfd, s)) {
    return false;
  }
  if (!BindSocketWithUsablePort(sockfd, port_start,
                                   port_start + max_retry - 1, interval,
                                   sockport, s)) {
    goto ERR_BIND;
  }
  fd   = sockfd;
  port = sockport;
  return true;

ERR_BIND:
  if (sockfd > 0) {
    close(sockfd);
    sockfd = -1;
  }
  return false;
}

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

bool PollSocket(int fd, int timeout /*in ms*/, status &s) {
  pollfd wfd[1];
  
  wfd[0].fd     = fd;
  wfd[0].events = POLLOUT;

  int rc = poll(wfd, 1, timeout);
  if (rc == -1) {
    SysCallErrRecoder(poll, s);
    s << ",fd=" << fd << ",poll_timeout=" << timeout;
    return false;
  }
  if (rc == 0) {
    s << "connection timeout: " << strerror(ETIMEDOUT);
    return false;
  }
  return true;
}

TcpAddrInfo::TcpAddrInfo(const std::string &ip, int port, status &s) : info(nullptr) {
  struct addrinfo hints;
  char            _port[8];

  snprintf(_port, 8, "%d", port);
  memset(&hints, 0, sizeof(hints));

  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // addrinfo
  int r = getaddrinfo(ip.c_str(), _port, &hints, &info);
  if (r != 0) {
    s << "call getaddrinfo failed: " << gai_strerror(r);
    info = nullptr;
    return;
  }
}

TcpAddrInfo::~TcpAddrInfo() {
  if (info) {
    freeaddrinfo(info);
  }
}

bool TcpAddrInfo::OK() { return info != nullptr; }

} // namespace io
} // namespace cp1craft
