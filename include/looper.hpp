#pragma once

// a c++ wrapper for libevent utilities

#include "event2/event.h"
#include "event2/listener.h"
#include "event2/util.h"

#include "log.h"
#include "utils.h"

namespace cp1craft {
namespace io {

class Looper;

short PersisReadableFlags() { return EV_PERSIST | EV_READ | EV_TIMEOUT; }
short PersisWritableFlags() { return EV_PERSIST | EV_WRITE | EV_TIMEOUT; }

// InternalEvent
template<class Callable>
class InternalEvent {
public:
  InternalEvent(struct event_base *base, int fd, short flags,
                cp1craft::utils::LoggerPtr logger)
      : ev_base_(base), fd_(fd), flags_(flags), ev_(nullptr), logger_(logger) {
    if (ev_base_ != nullptr) {
      ev_ = event_new(ev_base_, fd_, flags_,
                      InternalEvent<Callable>::LibeventCallback, this);
      if (ev_ == nullptr) {
        do_log(error, logger_, "call {event_new} failed: fd={}, flags={}", fd_,
               flags_);
      }
    }
  }

  ~InternalEvent() {
    if (ev_) {
      event_free(ev_);
    }
  }

  bool Enable(const struct timeval *tv = nullptr) {
    int rc = event_add(ev_, tv);
    if (rc != 0) {
      do_log(error, logger_, "call {event_add} failed: ev={}, retcode={}",
             (void *)ev_, rc);
    }
    return rc == 0;
  }

  bool Disable() {
    int rc = event_del(ev_);
    if (rc != 0) {
      do_log(error, logger_, "call {event_del} failed: ev={}, retcode={}",
             (void *)ev_, rc);
    }
    return rc == 0;
  }

private:
  struct event_base *        ev_base_;
  int                        fd_;
  short                      flags_;
  struct event *             ev_;
  cp1craft::utils::LoggerPtr logger_;
  static void LibeventCallback(int fd /**/, short flag, void *arg);
};

template<class Callable>
void InternalEvent<Callable>::LibeventCallback(int fd, short flags, void *arg) {
  InternalEvent<Callable> *p = reinterpret_cast<InternalEvent<Callable> *>(arg);
  do_log(warn, p->logger_, "would trigger event={}, fd={}, flags={}...",
         (void *)p->ev_, fd, flags);
  static_cast<Callable *>(p)->Trigger(fd, flags);
}
// InternalEvent

// CallableEvent
// Fn must be like:
//   Fn (int, short, Args args...)
//
template<typename Fn, typename... Args>
class CallableEvent : public InternalEvent<CallableEvent<Fn, Args...>> {
private:
  using internalevent = InternalEvent<CallableEvent<Fn, Args...>>;
  using signature     = void(int, short, Args...);
  static_assert(std::is_convertible<Fn &&, std::function<signature>>::value,
                "required callback forms: void(int, short, Args...)..");

  std::function<signature> ufn_;   // user callback
  std::tuple<Args...>      uargs_; // user parameters
public:
  //
  // call this means I am interesting with @fd 's @flags events, if that
  // happens, tell me by calling `fn(fd, flags, args...)`
  //

  CallableEvent(struct event_base *base, cp1craft::utils::LoggerPtr logger,
                int fd, short flags, Fn &&fn, Args &&... args)
      : internalevent(base, fd, flags, logger), ufn_(std::forward<Fn>(fn)),
        uargs_(std::forward<Args>(args)...) /*parameters unpack*/ {}

  CallableEvent(const CallableEvent<Fn, Args...> &) = delete;
  CallableEvent(CallableEvent<Fn, Args...> &&)      = delete;

  void Trigger(int fd, short flags) {
    call_user_callback(fd, flags,
                       typename cp1craft::utils::tuple_unpack_helper::gen_seq<
                           sizeof...(Args)>::type{});
  }

private:
  template<int... S>
  void call_user_callback(int fd, short flags,
                          cp1craft::utils::tuple_unpack_helper::index<S...>) {
    // before user callback is called...
    //
    ufn_(fd, flags, std::get<S>(uargs_)...);
    // after user callback is called...
    //
  }
};

template<typename Fn, typename... Args>
std::shared_ptr<CallableEvent<Fn, Args...>>
make_CallableEvent(struct event_base *base, cp1craft::utils::LoggerPtr logger,
                   int fd, short flags, Fn &&f, Args &&... args) {
  return std::make_shared<CallableEvent<Fn, Args...>>(
      base, logger, fd, flags, std::forward<Fn>(f),
      std::forward<Args>(args)...);
}

class Looper {
public:
  Looper(cp1craft::utils::LoggerPtr logger = cp1craft::utils::g_console);
  ~Looper();
  struct event_base *GetBase() {
    return ev_base_;
  }
  cp1craft::utils::LoggerPtr GetLogger() { return logger_; }
  bool                       StartLoop();
  bool                       StopLoop();

private:
  cp1craft::utils::LoggerPtr logger_;
  struct event_base *        ev_base_;
};

Looper::Looper(cp1craft::utils::LoggerPtr logger)
    : logger_(logger), ev_base_(nullptr) {
  ev_base_ = event_base_new();
  if (ev_base_ == nullptr) {
    do_log(error, logger, "failed to createloop: event_base_new");
  }
}
Looper::~Looper() {
  if (ev_base_) {
    event_base_free(ev_base_);
  }
}
bool Looper::StartLoop() {
  if (ev_base_) {
    int rc = event_base_dispatch(ev_base_);
    if (rc != 0) {
      do_log(error, logger_,
             "failed to startloop: event_base_dispatch return {}.", rc);
      return false;
    }
    return true;
  }
  do_log(error, logger_, "failed to startloop: evbase is null");
  return false;
}
bool Looper::StopLoop() {
  if (ev_base_) {
    int rc = event_base_loopexit(ev_base_, NULL);
    if (rc != 0) {
      do_log(error, logger_,
             "failed to stoploop: event_base_dispatch return {}.", rc);
      return false;
    }
    return true;
  }
  do_log(error, logger_, "failed to stoploop: evbase is null");
  return false;
}

template<typename Fn, typename... Args>
std::shared_ptr<CallableEvent<Fn, Args...>>
make_CallableEvent(Looper *lp, int fd, short flags, Fn &&f, Args &&... args) {
  if (lp != nullptr && lp->GetBase() != nullptr) {
    return std::make_shared<CallableEvent<Fn, Args...>>(
        lp->GetBase(), lp->GetLogger(), fd, flags, std::forward<Fn>(f),
        std::forward<Args>(args)...);
  }
  if (lp) {
    do_log(error, lp->GetLogger(),
           "failed to make event: looper={}, looper's base={}", (void *)lp,
           (void *)lp->GetBase());
  }
  return nullptr;
}

// definitions in libevent
/**
   Allocate a new evconnlistener object to listen for incoming TCP connections
   on a given file descriptor.

   @param base The event base to associate the listener with.
   @param cb A callback to be invoked when a new connection arrives.  If the
      callback is NULL, the listener will be treated as disabled until the
      callback is set.
   @param ptr A user-supplied pointer to give to the callback.
   @param flags Any number of LEV_OPT_* flags
   @param backlog Passed to the listen() call to determine the length of the
      acceptable connection backlog.  Set to -1 for a reasonable default.
      Set to 0 if the socket is already listening.
   @param fd The file descriptor to listen on.  It must be a nonblocking
      file descriptor, and it should already be bound to an appropriate
      port and address.
*/
// EVENT2_EXPORT_SYMBOL
// struct evconnlistener *evconnlistener_new(struct event_base *base,
//    evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
//    evutil_socket_t fd);
//
/**
    A callback that we invoke when a listener has a new connection.

    @param listener The evconnlistener
    @param fd The new file descriptor
    @param addr The source address of the connection
    @param socklen The length of addr
    @param user_arg the pointer passed to evconnlistener_new()
*/
//  typedef void (*evconnlistener_cb)(struct evconnlistener *, evutil_socket_t,
//  struct sockaddr *, int socklen, void *);
//

int GetDefaultListenFlags() {
  return LEV_OPT_CLOSE_ON_FREE |
         LEV_OPT_REUSEABLE /*address reusable, for quick restart*/;
}
int GetDefaultBacklog() { return -1; }

template<typename Listener>
class InternalListener {
public:
  InternalListener(struct event_base *base, int listenfd, int flags,
                   int backlog, cp1craft::utils::LoggerPtr logger)
      : ev_base_(base), listen_fd_(listenfd), ec_listener_(nullptr),
        logger_(logger) {
    if (ev_base_ != nullptr) {
      ec_listener_ = evconnlistener_new(
          ev_base_, InternalListener<Listener>::LibeventListenCallback, this,
          flags, backlog, listen_fd_);
      do_log(warn, logger_,
             "listener={} alloc. bound with {listenfd={}, base={}, flags={}, "
             "backlog={}}...",
             (void *)ec_listener_, listen_fd_, (void *)ev_base_, flags,
             backlog);
    }
  }
  ~InternalListener() {
    if (ec_listener_) {
      evconnlistener_free(ec_listener_);
      ec_listener_ = nullptr;
    }
  }

private:
  struct event_base *        ev_base_;
  int                        listen_fd_;
  struct evconnlistener *    ec_listener_;
  cp1craft::utils::LoggerPtr logger_;

  void LibeventListenCallback(struct evconnlistener *, evutil_socket_t,
                              struct sockaddr *, int, void *);
};

template<typename Listener>
void InternalListener<Listener>::LibeventListenCallback(
    struct evconnlistener *l, evutil_socket_t fd, struct sockaddr *sockaddr,
    int socklen, void *arg) {
  InternalListener<Listener> *p =
      reinterpret_cast<InternalListener<Listener> *>(arg);
  do_log(warn, p->logger_,
         "would trigger listener={}, newfd={}, sockaddr={}, socklen={}...",
         (void *)l, fd, (void *)sockaddr, socklen); // TODO - sockaddr printer
  static_cast<Listener *>(p)->Trigger(l, fd, sockaddr, socklen);
}

template<typename Fn, typename... Args>
class Listener : InternalListener<Listener<Fn, Args...>> {
private:
  using signature        = void(struct evconnlistener *, int /*connfd*/,
                         struct sockaddr * /*src addr*/, int, Args...);
  using internallistener = InternalListener<Listener>;
  static_assert(
      std::is_convertible<Fn &&, std::function<signature>>::value,
      "required callback forms: void(struct evconnlistener *, int /*connfd*/, "
      "struct sockaddr * /*src addr*/, int , Args...)..");
  std::function<signature> ufn_;   // user callback
  std::tuple<Args...>      uargs_; // user parameters
public:
  Listener(struct event_base *base, int listenfd, int flags, int backlog,
           cp1craft::utils::LoggerPtr logger, Fn &&fn, Args &&... args)
      : internallistener(base, listenfd, flags, backlog, logger),
        ufn_(std::forward<Fn>(fn)), uargs_(std::forward<Args>(args)...) {}

  Listener(const Listener<Fn, Args...> &) = delete;
  Listener(Listener<Fn, Args...> &&)      = delete;

  void Trigger(struct evconnlistener *listener, int newfd,
               struct sockaddr *sockaddr, int socklen) {
    call_user_callback(listener, newfd, sockaddr, socklen,
                       typename cp1craft::utils::tuple_unpack_helper::gen_seq<
                           sizeof...(Args)>::type{});
  }

private:
  template<int... S>
  void call_user_callback(struct evconnlistener *listener, int newfd,
                          struct sockaddr *sockaddr, int socklen,
                          cp1craft::utils::tuple_unpack_helper::index<S...>) {
    // before
    ufn_(listener, newfd, sockaddr, socklen, std::get<S>(uargs_)...);
    // after
  }
};

template<typename Fn, typename... Args>
std::shared_ptr<Listener<Fn, Args...>>
make_Listener(struct event_base *base, int listenfd, int flags, int backlog,
              cp1craft::utils::LoggerPtr logger, Fn &&fn, Args &&... args) {
  return std::make_shared<Listener<Fn, Args...>>(base, listenfd, flags, backlog,
                                                 logger, std::forward<Fn>(fn),
                                                 std::forward<Args>(args)...);
}

// with default settings of listen flags and backlog
template<typename Fn, typename... Args>
std::shared_ptr<Listener<Fn, Args...>>
make_Listener(struct event_base *base, int listenfd,
              cp1craft::utils::LoggerPtr logger, Fn &&fn, Args &&... args) {
  return std::make_shared<Listener<Fn, Args...>>(
      base, listenfd, GetDefaultListenFlags(), GetDefaultBacklog(), logger,
      std::forward<Fn>(fn), std::forward<Args>(args)...);
}

template<typename Fn, typename... Args>
std::shared_ptr<Listener<Fn, Args...>> make_Listener(Looper *lp, int listenfd,
                                                     Fn &&fn, Args &&... args) {
  if (lp != nullptr && lp->GetBase() != nullptr) {
    return std::make_shared<Listener<Fn, Args...>>(
        lp->GetBase(), listenfd, GetDefaultListenFlags(), GetDefaultBacklog(),
        lp->GetLogger(), std::forward<Fn>(fn), std::forward<Args>(args)...);
  }
}

} // namespace io
} // namespace cp1craft
