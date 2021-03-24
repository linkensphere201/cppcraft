#pragma once

// a c++ wrapper for libevent utilities

#include "event2/event.h"
#include "event2/listener.h"
#include "event2/util.h"

#include "log.h"

namespace cp1craft {
namespace io {

class Looper;

class EventInternal {
protected:
  static void LibeventCallback(evutil_socket_t, short, void *);
  struct event *ev_;
};

template<typename Fn, typename ... Args>
class Event : public EventInternal {
public:
  Event(Looper *lp, int fd, short flag, Fn&& ucallback, Args... uargs);
private:
};

class Looper {
public:
  Looper(cp1craft::utils::LoggerPtr logger = cp1craft::utils::g_console);
  ~Looper();
  bool StartLoop();
  bool StopLoop();

private:
  cp1craft::utils::LoggerPtr logger_;
  struct event_base *ev_base_;
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
      do_log(error, logger,
             "failed to startloop: event_base_dispatch return {}.", rc);
      return false;
    }
    return true;
  }
  do_log(error, logger, "failed to startloop: evbase is null");
  return false;
}
bool Looper::StopLoop() {
  if (ev_base_) {
    int rc = event_base_loopexit(ev_base_, NULL);
    if (rc != 0) {
      do_log(error, logger,
             "failed to stoploop: event_base_dispatch return {}.", rc);
      return false;
    }
    return true;
  }
  do_log(error, logger, "failed to stoploop: evbase is null");
  return false;
}
} // namespace io
} // namespace cp1craft
