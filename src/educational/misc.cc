// keywords: misc
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "misc");
}

TEST(misc, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module(); 
  return RUN_ALL_TESTS();
}
