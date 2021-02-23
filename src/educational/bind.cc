// keywords: bind
//

#include <gtest/gtest.h>
#include <algorithm> // for_each
#include <functional>
#include <vector>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "bind");
}

TEST(bind, ShowTopic) {
  ShowTopic();
}

/*
 * HOW DO THOSE FUNKY PLACEHOLDERS WORK?
 * https://accu.org/journals/overload/14/73/gibson_1397/
 *
 * (cn)std::bind的实现原理：
 * http://blog.guorongfei.com/2017/01/27/bind-implementation
 *
 * std::for_each:
 *  template<class InputIterator, class Function>
    Function for_each(InputIterator first, InputIterator last, Function fn)
    {
      while (first!=last) {
        fn (*first);
        ++first;
      }
      return fn;      // or, since C++11: return move(fn);
    }
 *
 * */

void unary_foo(int x) {
  log_info("output uf: {}", x);
}
void foo(int x, int y, int z) {
  log_info("output middle: {}", y);
}

void test_stdbind_basic() {
  
  std::vector<int> vl{10,20,30,40};
  for_each (vl.begin(), vl.end(), unary_foo);
  log_info("--------------");
  for_each (vl.begin(), vl.end(), std::bind(foo, 10, std::placeholders::_1, 1));
}




TEST(bind, basic) {
  test_stdbind_basic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
