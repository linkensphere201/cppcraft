// keywords: template_check
//


#include "event2/event.h"
#include "event2/listener.h"
#include "event2/util.h"

#include "looper.hpp"
#include "log.h"
#include "utils.h"

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

TEST(template_check, tuple_unpack) {
  auto f1 = [](int a, int b, int c, int d) {
    log_info("called! {}-{}-{}-{}", a, b, c, d);
  };
  auto c = cp1craft::utils::tuple_unpack_helper::make_callable(f1, 1, 2, 3, 4);
  c.call();
}

TEST(template_check, CallableEvent) {
  auto c = cp1craft::io::make_CallableEvent(
      (struct event_base*)nullptr, nullptr, 0, 0,
      [](int fd, short flags, int a, int b) {
        log_info("it happened! {},{}", a, b);
      },
      10, 20);
  c->Trigger(0, 0);
}

TEST(template_check, libevent) {
  struct event_base *evbase = event_base_new();
  int calltimes = 0;
  auto e = cp1craft::io::make_CallableEvent(
      evbase, cp1craft::utils::g_console, 0/*stdin*/, cp1craft::io::PersisReadableFlags(),
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

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
