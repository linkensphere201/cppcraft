#!/bin/bash

TOPIC=$1
code="// keywords: $TOPIC
//

#include <gtest/gtest.h>
#include <iostream>
#include \"log.h\"

void ShowTopic() {
  log_info(\"Topic: \" \"$TOPIC\");
}

TEST($TOPIC, ShowTopic) {
  ShowTopic();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  common::init_log_module(); 
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
