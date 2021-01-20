// keywords:
//

#define TOPIC "misc"
#include <gtest/gtest.h>
#include <iostream>

void ShowTopic() {
  std::cout << "topic: " << TOPIC << std::endl;
}

TEST(misc, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
