// keywords: inheritance
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "inheritance");
}

TEST(inheritance, ShowTopic) {
  ShowTopic();
}


class xbase {
  public:
    xbase(int i, const std::string &s) : i_(i), s_(s) {}
    void say() {
      log_info("i: {}, s: {}", i_, s_);
    }

  private:
    int i_;
    std::string s_;
};

class x : public xbase {
  public:
    x(int i, const std::string &s) : xbase(i, s) { }
};

TEST(x, util1) {
  x f1(1, "abc");
  f1.say(); // defined in it's base class
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
