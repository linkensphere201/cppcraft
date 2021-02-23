// keywords:
//

#define TOPIC "stdthread"
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "log.h"

void ShowTopic() {
  log_warn("topic: {}", TOPIC);
}

TEST(stdthread, ShowTopic) {
  ShowTopic();
}

void foo(int x) {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  log_info("tid {} output: {}", ss.str(), x);
}

TEST(stdthread, threadid) {
  std::vector<std::thread> threads;
  for (int i = 0; i < 3; i++) {
    auto t = std::thread(foo, i);
    std::stringstream ss;
    ss << t.get_id();
    log_info("tid: {}", ss.str());
    threads.push_back(std::move(t));
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  for (auto &i : threads) {
    i.join();
  }
}

int main(int argc, char **argv) {
  utils::init_log_module(); 
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
