// keywords: template_check
//

#include "log.h"

#include "event2/event.h"
#include "event2/listener.h"
#include "event2/util.h"

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>

void ShowTopic() {
  log_info("Topic: "
           "template_check");
}

TEST(template_check, ShowTopic) { ShowTopic(); }

namespace tuple_unpack_helper {
// for c++11
template<int... Is>
struct index {};

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template<int... Is>
struct gen_seq<0, Is...> {
  typedef index<Is...> type;
};

template<typename Fn, typename... Args>
struct callable {
  callable(Fn &&f, Args &&... a)
      : fn(std::forward<Fn>(f)), params(std::forward<Args>(a)...) {}
  std::function<void(Args...)> fn;
  std::tuple<Args...> params;
  void call() {
    return callFunc(
        typename gen_seq<sizeof...(Args)>::
            type{}); // sizeof... to fetch parameter size of a parameter pack
  }
  template<int... S>
  void callFunc(index<S...>) {
    fn(std::get<S>(params)...); // parameter unpack --> fn(std::get<0>(params),
                                // std::get<1>(params), ...);
  }
};
template<typename Fn, typename... Args>
callable<Fn, Args...> make_callable(Fn &&callback, Args &&... params) {
  return callable<Fn, Args...>(std::forward<Fn>(callback),
                               std::forward<Args>(params)...);
}
// for c++14, we can use std::integer_sequence instead
// for c++17, we can use std::apply or std::invoke instead
} // namespace tuple_unpack_helper

TEST(template_check, tuple_unpack) {
  auto f1 = [](int a, int b, int c, int d) {
    log_info("called! {}-{}-{}-{}", a, b, c, d);
  };
  auto c = tuple_unpack_helper::make_callable(f1, 1, 2, 3, 4);
  c.call();
}

// void LibeventCallback(int fd, short flag, void *arg);
short PersisReadableFlags() { return EV_PERSIST | EV_READ | EV_TIMEOUT; }
short PersisWritableFlags() { return EV_PERSIST | EV_WRITE | EV_TIMEOUT; }

template<class Callable>
class InternalEvent {
public:
  InternalEvent(struct event_base *base, int fd, short flags)
      : ev_base_(base), fd_(fd), flags_(flags), ev_(nullptr) {
    if (ev_base_ != nullptr) {
      ev_ = event_new(ev_base_, fd_, flags_,
                      InternalEvent<Callable>::LibeventCallback, this);
    }
  }

  ~InternalEvent() {
    if (ev_) {
      event_free(ev_);
    }
  }
  
  bool Enable(const struct timeval *tv = nullptr) {
    int rc = event_add(ev_, tv);
    return rc == 0;
  }

  bool Disable() {
    int rc = event_del(ev_);
    return rc == 0;
  }

private:
  struct event_base *ev_base_;
  int fd_;
  short flags_;
  struct event *ev_;
  static void LibeventCallback(int fd, short flag, void *arg);
};

template<class Callable>
void InternalEvent<Callable>::LibeventCallback(int fd, short flags, void *arg) {
  InternalEvent<Callable> *p = reinterpret_cast<InternalEvent<Callable> *>(arg);
  static_cast<Callable *>(p)->Trigger(fd, flags);
}

// Fn must be like:
//   Fn (extra_args*, Args args...)
//
template<typename Fn, typename... Args>
class CallableEvent : public InternalEvent<CallableEvent<Fn, Args...>> {
private:
  using internalevent = InternalEvent<CallableEvent<Fn, Args...>>;
  using signature = void(int, short, Args...);
  static_assert(std::is_convertible<Fn &&, std::function<signature>>::value,
                "wrong signature");

  std::function<signature> ufn_; // user callback
  std::tuple<Args...> uargs_;    // user function
public:
  //
  // call this means I am interesting with @fd 's @flags events, if that happens,
  // tell me by calling `fn(fd, flags, args...)`
  //

  CallableEvent(struct event_base *base, int fd, short flags, Fn &&fn,
                Args &&... args)
      : internalevent(base, fd, flags), ufn_(std::forward<Fn>(fn)),
        uargs_(std::forward<Args>(args)...) /*parameters unpack*/ {}
  
  CallableEvent(const CallableEvent<Fn, Args...> &) = delete;
  CallableEvent(CallableEvent<Fn, Args...> &&) = delete;

  void Trigger(int fd, short flags) {
    call_user_callback(
        fd, flags,
        typename tuple_unpack_helper::gen_seq<sizeof...(Args)>::type{});
  }

private:
  template<int... S>
  void call_user_callback(int fd, short flags,
                          tuple_unpack_helper::index<S...>) {
    // before user callback is called...
    //
    ufn_(fd, flags, std::get<S>(uargs_)...);
    // after user callback is called...
    //
  }
};

template<typename Fn, typename... Args>
std::shared_ptr<CallableEvent<Fn, Args...>> make_CallableEvent(struct event_base *base, int fd,
                                              short flags, Fn &&f,
                                              Args &&... args) {
  return std::make_shared<CallableEvent<Fn, Args...>>(base, fd, flags, std::forward<Fn>(f),
                                    std::forward<Args>(args)...);
}

TEST(template_check, CallableEvent) {
  auto c = make_CallableEvent(
      nullptr, 0, 0,
      [](int fd, short flags, int a, int b) {
        log_info("it happened! {},{}", a, b);
      },
      10, 20);
  c->Trigger(0, 0);
}

TEST(template_check, libevent) {
  struct event_base *evbase = event_base_new();
  int calltimes = 0;
  auto e = make_CallableEvent(evbase, 0/*stdin*/, PersisReadableFlags(), 
      [](int fd, short flags, int& calltimes) {
        char letter;
	      int n = read(fd, &letter, 1);
        log_info("fd={}, caught event={}, calltime={}, letter={:#x}", fd, flags, calltimes, letter);
        calltimes++;
      },
      calltimes
      );

  e->Enable();
  event_base_dispatch(evbase);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
