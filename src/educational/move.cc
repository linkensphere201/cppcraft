// keywords: move
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "move");
}

TEST(move, ShowTopic) {
  ShowTopic();
}
// - Is *move* really move something ?
//  https://stackoverflow.com/questions/39029145/how-does-a-move-constructor-work
// - what-the-fuck is move sementics ?
//  https://stackoverflow.com/questions/3106110/what-is-move-semantics
//
// - https://stackoverflow.com/questions/17642357/const-reference-vs-move-semantics

// https://stackoverflow.com/questions/38287425/moved-objects-are-still-destructed

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module(); 
  return RUN_ALL_TESTS();
}
