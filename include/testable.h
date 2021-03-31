#pragma once

#include "log.h"

namespace testable {
struct T1 {
  T1() : i_(-1) {}
  T1(int x) : i_(x) { log_info("ctor"); }
  T1(T1&& t) : i_(t.i_) {
    t.i_ = -1;
    log_info("mv-ctor");
  }
  T1& operator=(T1&& t) {
    i_ = t.i_;
    t.i_ = -1;
    log_info("mv-assign");
    return *this;
  }

  int get() { return i_; }
  int i_;
};
}  // namespace testable
