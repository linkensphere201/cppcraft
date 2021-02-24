// keywords: skiplist
//

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "log.h"
#include "skiplist.hpp"

void ShowTopic() {
  log_info("Topic: " "skiplist");
}

TEST(skiplist, ShowTopic) {
  ShowTopic();
}

TEST(comparator, _int_) {
  ASSERT_TRUE((*GetDefComparator<int>())(1, 2) < 0) << "cmp(1,2) < 0";
  ASSERT_EQ((*GetDefComparator<int>())(1, 1),0) << "cmp(1,1) == 0";
  ASSERT_TRUE((*GetDefComparator<int>())(2, 1) > 0) << "cmp(2,1) > 0";
}

TEST(comparator, _string_) {
  ASSERT_TRUE((*GetDefComparator<std::string>())("1", "2") < 0) << "cmp(1,2) < 0";
}

#include <chrono>
#include <sstream>
#include <fmt/core.h>
TEST(fmt, examples) {
  int a;
  std::string s = fmt::format("{:p}", (void*)&a);
  log_info("{}", s);
}


/*
 * syntax of formats:
 * https://fmt.dev/latest/syntax.html
 * */

TEST(skiplist, getlevel) {
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  Skiplist<int> skl(seed);
  int LvlCount[Skiplist<int>::kMaxLvl_] = {0};
  for (int i = 0; i < 100; i++) {
    LvlCount[skl.GetLevel()]++;
  }

  std::stringstream ss;
  log_info("branching factor: {}", ((double)Skiplist<int>::kP_ / (double)Skiplist<int>::kPMax_));

  log_info("level counts:");
  for (int i = 0; i < Skiplist<int>::kMaxLvl_; i++) {
    ss << fmt::format("{:<3d} |", i);
  }
  log_info("{}", ss.str());
  
  ss.str("");
  for (auto & i : LvlCount) {
    ss << fmt::format("{:<3d} |", i);
  }
  log_info("{}", ss.str());
}

//
TEST(Node, tostring) {
  Skiplist<std::string>::Node s(0, "mytest");
  log_info("{}", s.ToString());
}

TEST(keyhandle, cmp) {
  SimpleKeyHandle a(0, nullptr), b(1, nullptr);
  log_info("a < b: {}", std::less<SimpleKeyHandle>()(a, b));
  ASSERT_TRUE(std::less<SimpleKeyHandle>()(a, b));
}

TEST(keyhandle, tostring) {
  SimpleKeyHandle a(23, (void*)0xdeadbeef);
  log_info("{}", fmt::format("{}", a));
}

TEST(skiplist, insert5) {
  Skiplist<SimpleKeyHandle> skl;
  int* xp = (int*)0x0;
  skl.Insert(SimpleKeyHandle(10, xp));
  skl.Insert(SimpleKeyHandle(20, xp++));
  skl.Insert(SimpleKeyHandle(40, xp++));
  skl.Insert(SimpleKeyHandle(30, xp++));
  skl.Insert(SimpleKeyHandle(50, xp++));

  std::string s;
  skl.DumpAsGraphvizScatch(false, s);
  log_info("after insertion:");
  log_info("{}", s);
}

TEST(skiplist, insertx) {
  Skiplist<SimpleKeyHandle> skl;
  int* xp = (int*)0x0;
  
  std::mt19937 generator(25);
  std::uniform_int_distribution<int> dist(10, 200);

  int max = 15;
  for (int i = 0; i < max; i++) {
    int a = dist(generator);
    skl.Insert(SimpleKeyHandle(a, xp++));
  }
  std::string s;
  skl.DumpAsGraphvizScatch(false, s);
  log_info("after insertion:");
  log_info("{}", s);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
