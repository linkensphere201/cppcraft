// keywords: type_traits
//

#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "type_traits");
}

TEST(type_traits, ShowTopic) {
  ShowTopic();
}


// 
// https://stackoverflow.com/questions/38067106/c-verify-callable-signature-of-template-type
//

#include <functional>
#include <type_traits>
// Route only work if f is in this form: `int (*) (void)`, return a int and take no arguements.
using ValidFunc = std::function<int(void)>;
template<typename Func,
  typename std::enable_if<std::is_convertible<Func&, ValidFunc>::value>::type>
int Route(Func f) {
  return f();
}

int ta(void) {
      log_info("valid func");  
      return 100;
}

TEST(Route, enable_if) {
  auto a = [](void)->int{
      log_info("valid func");  
      return 100;
  };
  // Route(a);
  // Route(ta);
  log_info("{}", std::is_convertible<decltype(ta), ValidFunc>::value)
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
