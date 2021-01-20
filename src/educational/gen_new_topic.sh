#!/bin/bash

TOPIC=$1
code="// keywords:
//

#define TOPIC \"$TOPIC\"
#include <gtest/gtest.h>
#include <iostream>

void ShowTopic() {
  std::cout << \"topic: \" << TOPIC << std::endl;
}

TEST($TOPIC, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}"

function usage() {
  echo "$0 <cpp_topic>"
}

if [ "$TOPIC" == "" ]; then
  usage
  exit -1
fi

echo "$code" > $TOPIC.cc
