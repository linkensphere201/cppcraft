// keywords: skiplist
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"
#include "skiplist.hpp"

void ShowTopic() {
  log_info("Topic: " "skiplist");
}

TEST(skiplist, ShowTopic) {
  ShowTopic();
}

TEST(comparator, basic) {
  ASSERT_TRUE((*GetDefComparator<int>())(1, 2) < 0) << "cmp(1,2) < 0";
  ASSERT_EQ((*GetDefComparator<int>())(1, 1),0) << "cmp(1,1) == 0";
  ASSERT_TRUE((*GetDefComparator<int>())(2, 1) > 0) << "cmp(2,1) > 0";
}

#include <chrono>
#include <sstream>
TEST(skiplist, getlevel) {
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  Skiplist<int> skl(seed);
  int LvlCount[Skiplist<int>::kMaxLvl_] = {0};
  for (int i = 0; i < 100; i++) {
    LvlCount[skl.GetLevel()]++;
  }

  std::stringstream ss;
  log_info("branching factor: {}", ((double)Skiplist<int>::kP_ / (double)Skiplist<int>::kPMax_));
  for (auto & i : LvlCount) {
    ss << i << ",";
  }
  log_info("lvl count: {}", ss.str());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
