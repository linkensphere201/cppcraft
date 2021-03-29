#include "posix_ops.h"

#include <unistd.h>
#include <fcntl.h>
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
bool CreateTcpSocketandConnect(const std::string &ip, int port,
                               int conn_timeout, bool nonblock, int &fd,
                               status &s) {
  TcpAddrInfo addr(ip, port, s);
  if (!addr.OK()) {
    goto ERR_ADDRHINT;
  }
  if (!SetSocketNonblock(addr.sockfd, true, s)) {
    goto ERR_SETNONBLOCK;
  }

  int ret;
  ret = connect(addr.sockfd, addr.info->ai_addr, addr.info->ai_addrlen);
  if (ret == -1) {
    if (errno == EINPROGRESS) {
      if (!PollSocket(addr.sockfd, addr.info, s)) {
        goto ERR_CONN;
      }
    } else {
      SysCallErrRecoder(connect, s);
      goto ERR_CONN;
    }
  }
  if (!nonblock && !SetSocketNonblock(addr.sockfd, false, s)) {
    goto ERR_SETBLOCK;
  }
  
  fd = addr.sockfd;
  return true;


ERR_SETBLOCK:
ERR_CONN:
ERR_SETNONBLOCK:
  if (addr.sockfd > 0) {
    close(addr.sockfd);
    addr.sockfd = -1;
  }
ERR_ADDRHINT:
  return false;
}
bool CreateSocketforListen(bool reuse, int &fd, status &s) {
  int sockfd = -1;
  if (!CreateSocket(AF_INET, SOCK_STREAM, 0, sockfd, s)) {
    goto ERR_SOCK;
  }
  if (!SetSocketNonblock(sockfd, true, s)) {
    goto ERR_SETNONBLOCK;
  }
  int flags = 1;
  std::vector<int> optnames;
  optnames.push_back(SO_KEEPALIVE);

#if defined(SO_REUSEADDR)
  optnames.push_back(SO_REUSEADDR);
#endif

#if defined(SO_REUSEPORT)
  optnames.push_back(SO_REUSEPORT);
#endif

  int rc;
  for (auto sopt : optnames) {
    if (sopt == SO_REUSEPORT && !reuse) {
      continue;
    }
    rc = setsockopt(fd, SOL_SOCKET, sopt, (void*)&flags, (socklen_t)sizeof(flags));
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

bool CreateTcpSocketandBindPort(int port_start, int max_retry, int interval, int &fd, int &sockport, status &s) {
  int sockfd = -1;
  if (!CreateTcpSocketforListen(false, sockfd, s)) {
    return false;
  }
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);

  int port = port_start, attempts = 0;
  do {
    sin.sin_port = htons(port);
    int rc = bind(sockfd, (struct sockaddr *)&sin, sizeof(sin));
    if (rc == 0) {
      fd = sockfd;
      sockport = port;
      return true;
    }
    if (rc != 0 && errno != EADDRINUSE) {
      SysCallErrRecoder(bind, s);
      s << ",fd=" << sockfd;
      goto ERR_BIND;
    }

    usleep(interval); 
    port++;
    attempts++;
    max_retry--;
  } while(max_retry > 0);

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

bool PollSocket(int fd, int timeout/*in ms*/, struct addrinfo *info, status &s) {
  struct pollfd wfd[1];
  wfd[0].fd = fd;
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
  
  // copy sockaddr
  struct sockaddr *socka = (struct sockaddr *)malloc(info->ai_addrlen);
  if (socka == nullptr) {
    SysCallErrRecoder(malloc, s);
    return false;
  }
  memcpy(socka, info->ai_addr, info->ai_addrlen);
  
  bool ret = false;
  rc = connect(fd, (const struct sockaddr *)ai_addr, ai_addrlen);
  if (rc == 0)  {
    ret = true;
  } else if (errno == EISCONN) {
    ret = true;
  } else if (errno == EALREADY || errno == EINPROGRESS || errno == EWOULDBLOCK) {
    s << "connection timeout: " << strerror(ETIMEDOUT);
  } else {
    s << "connection failed: " << strerror(errno);
  }
  if (socka) {
    free(socka);
  }
  return ret;
}

TcpAddrInfo::TcpAddrInfo(const std::string &ip, int port, status &s)
    : info(nullptr), sockfd(-1) {
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
}

} // namespace io
} // namespace cp1craft
