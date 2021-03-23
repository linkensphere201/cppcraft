#pragma once

#include "log.h"

namespace cp1craft {
namespace io {

class Looper {
public:
  Looper(cp1craft::utils::LoggerPtr logger = cp1craft::utils::g_console)
      : logger_(logger), looper_(nullptr) {
    looper_ = hloop_new(0);
    if (looper_ == nullptr) {
      logger->error("fail to call {hloop_new}");
    }
  }
  ~Looper() { hloop_free(&looper_); }
  void StartLoop() {
    hloop_run(looper_);
  }
  void StopLoop() {
    hloop_stop(looper_);
  }
  void PauseLoop() {
    hloop_pause(looper_);
  }
  void ResumeLoop() {
    hloop_resume(looper_);
  }

private:
  cp1craft::utils::LoggerPtr logger_;
  hloop_t *looper_;
};

} // namespace io
} // namespace cp1craft
