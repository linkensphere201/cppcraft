// keywords: virtual
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "DesignPattern");
}

TEST(virtual, ShowTopic) {
  ShowTopic();
}

class tSomeModule {
public:
  virtual void someinf() = 0;
  bool sp;
  tSomeModule() : sp(true) {}
};

std::shared_ptr<tSomeModule> NewModule();

class tSomeImpl : public tSomeModule {
public:
  virtual void someinf() override;
};

void tSomeImpl::someinf() {
  log_info("simexxx: {} ...", sp);
}

std::shared_ptr<tSomeModule> NewModule() {
  return std::make_shared<tSomeImpl>();
}

void TestVirtual() {
  auto p = NewModule();
  p->someinf();
}

TEST(testvirtual, sol) {
  TestVirtual();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
