// keywords: bst
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "bst");
}

TEST(bst, ShowTopic) {
  ShowTopic();
}

struct bstnode {
  int val;
  bstnode *left;
  bstnode *right;
};



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
