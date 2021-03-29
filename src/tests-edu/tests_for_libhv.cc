// keywords: tests_for_libhv
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "tests_for_libhv");
}

TEST(tests_for_libhv, ShowTopic) {
  ShowTopic();
}

TEST(tests_for_libhv, timerloop) {
}

TEST(tests_for_libhv, acceptloop) {
}

TEST(tests_for_libhv, rdwrloop) {
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
