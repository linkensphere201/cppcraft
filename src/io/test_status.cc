#include <gtest/gtest.h>
#include <errno.h>
#include "log.h"
#include "status.h"
#include "fmt/core.h"


//
// syntax of formats:
//  https://fmt.dev/latest/syntax.html
//
TEST(status, basic) {
  cp1craft::utils::status s;
  s << "read error:" << strerror(EAGAIN);
  log_info("error: {}", s.value());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
