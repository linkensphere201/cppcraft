// keywords: bind
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "bind");
}

TEST(bind, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  common::init_log_module(); 
  return RUN_ALL_TESTS();
}
