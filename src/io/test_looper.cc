#include "log.h"
#include "looper.hpp"
#include "utils.h"
#include <gtest/gtest.h>

TEST(looper, callable) {
  auto f1 = [](int a, int b, int c, int d) {
    log_info("called! {}-{}-{}-{}", a, b, c, d);
  };
  auto c = cp1craft::utils::tuple_unpack_helper::make_callable(f1, 1, 2, 3, 4);
  c.call();
}

TEST(looper, event) {
  struct event_base *evbase = event_base_new();
  int calltimes = 0;
  auto e = cp1craft::io::make_CallableEvent(
      evbase, cp1craft::utils::g_console, 0 /*stdin*/, cp1craft::io::PersisReadableFlags(),
      [](int fd, short flags, int &calltimes) {
        char letter;
        int n = read(fd, &letter, 1);
        log_info("fd={}, caught event={}, calltime={}, letter={:#x}", fd, flags,
                 calltimes, letter);
        calltimes++;
      },
      calltimes);

  e->Enable();
  event_base_dispatch(evbase);
}

TEST(looper, looper) {
  cp1craft::io::Looper l;

  int calltimes = 0;
  auto e = cp1craft::io::make_CallableEvent(
      &l, 0, cp1craft::io::PersisReadableFlags(),
      [](int fd, short flags, int &calltimes, cp1craft::io::Looper *lp) {
        char letter;
        int n = read(fd, &letter, 1);
        log_info("fd={}, caught event={}, calltime={}, letter={:#x}", fd, flags,
                 calltimes, letter);
        calltimes++;
        if (lp && calltimes > 20) {
          log_info("recv {} bytes, byebye...", calltimes);
          lp->StopLoop();
        }
      },
      calltimes, &l);
  e->Enable();
  l.StartLoop();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
