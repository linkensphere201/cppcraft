// keywords: mod_tester
//

#include <gtest/gtest.h>
#include <iostream>
#include <functional>
#include "log.h"
#include "buffer.h"

void ShowTopic() {
  log_info("Topic: " "mod_tester");
}

TEST(mod_tester, ShowTopic) {
  ShowTopic();
}

using namespace cp1craft::io;
using uchar_t = Extent::uchar_t;
TEST(Extent, basic) {
  uchar_t *p = (uchar_t*)malloc(100);
  auto cdel = [](uchar_t*x){
    free(x);
  };
  
  Extent e0(p, 100, cdel);
  Extent e1(new uchar_t [64], 64, [](uchar_t*x) { delete [] x; });
}


TEST(Extent, move) {
  auto non_move_try1 = [](const Extent & ex) {
    //xxx
  };
  auto non_move_try2 = [](Extent & ex) {
    //xxx
  };
  auto non_move_try3 = [](Extent ex) {
    //xxx
  };
  auto do_move = [](Extent && ex) {
    //xxx
  };
  auto do_del = [] (uchar_t *x) {
    log_info("would delete {}...", (void*)x);
  };

  // example 1
  // non_move_try2(Extent(new uchar_t [64], 64, do_del)); 
  // compile error: tempory variable is rvalue
  
  // example 2
  // Extent ex(new uchar_t [64], 64, do_del);
  // non_move_try3(ex); 
  // involve a copy of temperory variable which would cause a double-free.
  
  // example 3
  // Extent ex(new uchar_t [64], 64, do_del);
  // do_move(std::move(ex));
  // in whole scope there would be only one object
  
  Extent ex(new uchar_t [64], 64, do_del);
  Extent ex2(std::move(ex));
  // ex(the object who is moved-from) & ex2 would both be release.
}

// how to move an object into & out of a container?
//
TEST(Move, Container) {

}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
