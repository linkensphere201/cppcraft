// keywords: skiplist
//

#include "log.h"
#include "skiplist.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

void ShowTopic() {
  log_info("Topic: "
           "skiplist");
}

TEST(skiplist, ShowTopic) { ShowTopic(); }

TEST(comparator, _int_) {
  ASSERT_TRUE((*cp1craft::utils::GetDefComparator<int>())(1, 2) < 0) << "cmp(1,2) < 0";
  ASSERT_EQ((*cp1craft::utils::GetDefComparator<int>())(1, 1), 0) << "cmp(1,1) == 0";
  ASSERT_TRUE((*cp1craft::utils::GetDefComparator<int>())(2, 1) > 0) << "cmp(2,1) > 0";
}

TEST(comparator, _string_) {
  ASSERT_TRUE((*cp1craft::utils::GetDefComparator<std::string>())("1", "2") < 0)
      << "cmp(1,2) < 0";
}

#include <chrono>
#include <fmt/core.h>
#include <sstream>
TEST(fmt, examples) {
  int a;
  std::string s = fmt::format("{:p}", (void *)&a);
  log_info("{}", s);
}

/*
 * syntax of formats:
 * https://fmt.dev/latest/syntax.html
 * */

TEST(skiplist, getlevel) {
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  cp1craft::utils::Skiplist<int> skl(seed);
  int LvlCount[cp1craft::utils::Skiplist<int>::kMaxLvl_] = {0};
  for (int i = 0; i < 100; i++) {
    LvlCount[skl.GetLevel()]++;
  }

  std::stringstream ss;
  log_info("branching factor: {}",
           ((double)cp1craft::utils::Skiplist<int>::kP_ / (double)cp1craft::utils::Skiplist<int>::kPMax_));

  log_info("level counts:");
  for (int i = 0; i < cp1craft::utils::Skiplist<int>::kMaxLvl_; i++) {
    ss << fmt::format("{:<3d} |", i);
  }
  log_info("{}", ss.str());

  ss.str("");
  for (auto &i : LvlCount) {
    ss << fmt::format("{:<3d} |", i);
  }
  log_info("{}", ss.str());
}

//
TEST(Node, tostring) {
  int lvl = 10, id = 10010;
  cp1craft::utils::Skiplist<std::string>::Node s(lvl, "mytest", id);
  log_info("node_id:{} ; node_content:{}", s.GetId(), s.ToString());
}

TEST(keyhandle, cmp) {
  cp1craft::utils::SimpleKeyHandle a(0, nullptr), b(1, nullptr);
  log_info("a < b: {}", std::less<cp1craft::utils::SimpleKeyHandle>()(a, b));
  ASSERT_TRUE(std::less<cp1craft::utils::SimpleKeyHandle>()(a, b));
}

TEST(keyhandle, tostring) {
  cp1craft::utils::SimpleKeyHandle a(23, (void *)0xdeadbeef);
  log_info("{}", fmt::format("{}", a));
}

TEST(skiplist, insert5) {
  cp1craft::utils::Skiplist<cp1craft::utils::SimpleKeyHandle> skl;
  int *xp = (int *)0x0;
  skl.Insert(cp1craft::utils::SimpleKeyHandle(10, xp));
  skl.Insert(cp1craft::utils::SimpleKeyHandle(20, xp++));
  skl.Insert(cp1craft::utils::SimpleKeyHandle(40, xp++));
  skl.Insert(cp1craft::utils::SimpleKeyHandle(30, xp++));
  skl.Insert(cp1craft::utils::SimpleKeyHandle(50, xp++));

  std::string s;
  skl.DumpAsGraphvizScatch(false, s);
  log_info("after insertion:");
  log_info("{}", s);
}

TEST(skiplist, insertx) {
  cp1craft::utils::Skiplist<cp1craft::utils::SimpleKeyHandle> skl;
  int *xp = (int *)0x0;

  std::mt19937 generator(25);
  std::uniform_int_distribution<int> dist(10, 200);

  int max = 15;
  for (int i = 0; i < max; i++) {
    int a = dist(generator);
    skl.Insert(cp1craft::utils::SimpleKeyHandle(a, xp++));
  }
  std::string s;
  skl.DumpAsGraphvizScatch(false, s);
  log_info("after insertion:");
  log_info("{}", s);
}

#include <map>
void init_skl(int seed, int keymin, int keymax, int count,
              cp1craft::utils::Skiplist<cp1craft::utils::SimpleKeyHandle> &skl,
              std::map<int, cp1craft::utils::SimpleKeyHandle> &ext_store /*external_storage*/) {
  std::mt19937 generator(seed);
  std::uniform_int_distribution<int> dist(keymin, keymax);
  int *fake_resource = (int *)0x0;
  for (int i = 0; i < count; i++) {
    int k = dist(generator);
    auto res = ext_store.emplace(k, cp1craft::utils::SimpleKeyHandle(k, fake_resource++));
    if (res.second) {
      skl.Insert(cp1craft::utils::SimpleKeyHandle(k, fake_resource++));
    }
  }
}

TEST(skiplist, search) {
  cp1craft::utils::Skiplist<cp1craft::utils::SimpleKeyHandle> skl;
  std::map<int, cp1craft::utils::SimpleKeyHandle> ext;
  init_skl(101, 10, 100, 50, skl, ext);
  std::string graph;
  skl.DumpAsGraphvizScatch(false, graph);
  // log_info("skl after init:");
  // log_info("{}", graph);

  int notfound = 150;
  bool called = false;
  auto fhook = [&called](const cp1craft::utils::SimpleKeyHandle &in, cp1craft::utils::SimpleKeyHandle &out) {
    out = in; // or `move` looks better.
    called = true;
  };

  cp1craft::utils::SimpleKeyHandle keyhandle(notfound, (int *)0x0);
  ASSERT_FALSE(skl.Search(keyhandle, fhook));
  ASSERT_FALSE(called);

  for (auto &itr : ext) {
    called = false;
    bool res = skl.Search(itr.second, fhook);
    ASSERT_TRUE(res) << "key <" << itr.first << "> is not found";
    ASSERT_TRUE(called);
  }
}

TEST(skiplist, remove) {
  cp1craft::utils::Skiplist<cp1craft::utils::SimpleKeyHandle> skl;
  std::map<int, cp1craft::utils::SimpleKeyHandle> ext;
  init_skl(101, 10, 100, 50, skl, ext);
  std::string graph;
  skl.DumpAsGraphvizScatch(false, graph);
  // log_info("skl after init:");
  // log_info("{}", graph);
  
}

namespace lc1206 {
class Skiplist {
public:
    Skiplist() : skl_(1, cp1craft::utils::Skiplist<int>::Policy::kKeyAllowedMulti){
    }
    bool search(int target) {
        return skl_.Search(target, nullptr);
    }
    void add(int num) {
        skl_.Insert(num);
    }
    bool erase(int num) {
        return skl_.Remove(num, nullptr);
    }
    cp1craft::utils::Skiplist<int> skl_;
};
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
