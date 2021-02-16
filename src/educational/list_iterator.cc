// keywords: list_iterator
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "list_iterator");
}

TEST(list_iterator, ShowTopic) {
  ShowTopic();
}
#include <sstream>
#include <deque>
void test_list_iteartor_validation() {
  auto print_deque = [](const std::deque<int> &rs) {
    std::stringstream ss;
    for (auto &i : rs) {
      ss << i << " - ";
    }
    log_info("{}", ss.str());
  };
  
  auto move_front = [](std::deque<int> &rs, std::deque<int>::iterator pos) -> std::deque<int>::iterator {
    int key = *pos;
    rs.erase(pos);
    rs.emplace_front(key);
    return rs.begin();
  };


  std::deque<int> li;
  std::deque<int>::iterator idxer[3];

  li.emplace_front(1);
  idxer[0] = li.begin();

  li.emplace_front(2);
  idxer[1] = li.begin();

  li.emplace_front(3);
  idxer[2] = li.begin();

  log_info("0 - {}, 1 - {}, 2 - {}", *idxer[0], *idxer[1], *idxer[2]);

  print_deque(li);

  log_info("after move:");
  auto old_iter = idxer[1];
  idxer[1] = move_front(li, idxer[1]);
  print_deque(li);
  
  log_info("old: {}, new: {}", *old_iter, *idxer[1]);
}
TEST(test_mv, move_by_pos) {
  test_list_iteartor_validation();
}

#include <utility>
#include <list>
#include "testable.h"

void test_list_move() {
  std::list<testable::T1> x;
  auto plist = [](std::list<testable::T1> &kl) {
    log_info("***");
    for (auto &i : kl) {
      log_info("{},", i.get());
    }
  };
  
  x.emplace_back(1);
  x.emplace_back(2);
  x.emplace_back(3);
  x.emplace_back(4);
  
  auto pos1 = x.begin(); 
  pos1++; // navi to 2

  auto pos2 = x.end();
  pos2--; // navi to 3
  pos2--;

  plist(x);
  
  log_info("---exchange by list::splice--");
  log_info("-----");
  x.splice(pos1, x, pos2); // no ctor is called
  plist(x);
  
  log_info("{} {}", pos1->get(), pos2->get()); // iterator is valid
  
  log_info("---exchange by std::swap--");
  std::swap(*pos1, *pos2);
  plist(x);
}

TEST(test_mv, move_by_splice) {
  test_list_move();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module(); 
  return RUN_ALL_TESTS();
}
