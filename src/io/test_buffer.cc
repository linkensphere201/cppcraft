#include <gtest/gtest.h>
#include <iostream>
#include "log.h"
#include "buffer.h"

using namespace cp1craft::io;

TEST(basic, uint32) {
  auto b = Buffer::NewBuffer();
  std::vector<uint32_t> inputs = {1187, 127, 221, 0x1b, 0xcb, 0xcf};
  for (auto i : inputs) {
    b->PutUint32(i);
  }
  int i = 0;
  while (true) {
    uint32_t got;
    if (b->PeekUint32(got)) {
      ASSERT_EQ(got, inputs[i]);
    } else {
      break;
    }
    i++;
  }
}

TEST(basic, string) {
  auto b = Buffer::NewBuffer();
  std::vector<std::string> inputs = {"abc", "testkj", "fuck"};
  std::vector<size_t> inputl = {inputs[0].size(), inputs[1].size(), inputs[2].size()};
  for (auto i : inputs) {
    b->PutString(i);
  }
  int i = 0;
  while (true) {
    std::string got;
    if (b->PeekString(inputl[i], got)) {
      ASSERT_EQ(got, inputs[i]);
    } else {
      break;
    }
    i++;
  }
}

TEST(basic, bytesarray) {
  auto b = Buffer::NewBuffer();
  auto b2 = Buffer::NewBuffer();
  std::vector<uint32_t> inputs = {1187, 127, 221, 0x1b, 0xcb, 0xcf};
  for (auto i : inputs) {
    b->PutUint32(i);
  }
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
