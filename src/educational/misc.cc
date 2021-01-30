// keywords: misc
//  - keyword default/delete
//    https://www.geeksforgeeks.org/explicitly-defaulted-deleted-functions-c-11/
//  - keyword virtual/override/final
//    https://stackoverflow.com/questions/13880205/is-the-override-keyword-just-a-check-for-a-overridden-virtual-method
//    https://www.fluentcpp.com/2020/02/21/virtual-final-and-override-in-cpp/
//   virtual

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "misc");
}

TEST(misc, ShowTopic) {
  ShowTopic();
}

/*
 *  `override` basic usage:
 *    - mark override in a derived function
 *    -  
 * */

class Base {
public:
  virtual void dosome();
};

class Xderived : public Base {
public:
  void dosome() override; // note that this function is override base class
  // void dosome() const override; 
    // compile failed: error: ‘void Xderived::dosome() const’ marked ‘override’, but does not override
    // because of no function found in base class
};
void Base::dosome() {
  log_info("default func!");
}

void Xderived::dosome() {
  log_info("derived func!");
}

TEST(misc, keyword_override) {
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module(); 
  return RUN_ALL_TESTS();
}
