// keywords: leetcode
//

#include <gtest/gtest.h>

#include <iostream>

#include "log.h"

void ShowTopic() {
  log_info(
      "Topic: "
      "leetcode 1 5");
}

TEST(leetcode, ShowTopic) { ShowTopic(); }

using namespace std;

#include <string>
#define MAX 1005

namespace lc5 {
static int subl[MAX][MAX];
static int stofs[MAX][MAX];
class Solution {
 public:
  string longestPalindrome(string s) {
    memset(subl, 0, sizeof(subl));
    memset(stofs, 0, sizeof(stofs));
    int len = s.length();
    for (int i = 1; i <= len; i++) {
      subl[i][i] = 1;
      stofs[i][i] = i;
    }
    for (int sc = 2; sc <= len; sc++) {
      for (int i = 1; i <= len; i++) {
        int j = i + sc - 1;
        if (j <= len) {
          int m = 0;
          if (s[i - 1] == s[j - 1] && subl[i + 1][j - 1] == (j - i - 1)) {
            m = subl[i + 1][j - 1] + 2;
          }
          if (m > subl[i + 1][j] && m > subl[i][j - 1]) {
            subl[i][j] = m;
            stofs[i][j] = i;
          } else if (subl[i + 1][j] > subl[i][j - 1]) {
            subl[i][j] = subl[i + 1][j];
            stofs[i][j] = stofs[i + 1][j];
          } else {
            subl[i][j] = subl[i][j - 1];
            stofs[i][j] = stofs[i][j - 1];
          }
        }
      }
    }
    std::string target = s.substr(stofs[1][len] - 1, subl[1][len]);
    // printf("origin: %s \n"
    //       "target: %s \n", s.c_str() , target.c_str());
    return target;
  }
};
}  // namespace lc5

TEST(longestPalindrome, sol1) {
  lc5::Solution s;
  s.longestPalindrome("aabaabaa");
}

#include <unordered_map>
namespace lc1 {
class Solution {
 public:
  vector<int> twoSum(vector<int>& nums, int target) {
    std::unordered_map<int, int> m;
    vector<int> res;
    for (int i = 0; i < nums.size(); i++) {
      int l = target - nums[i];
      auto x = m.find(l);
      if (x != m.end() && x->second != i) {
        res.emplace_back(i);
        res.emplace_back(x->second);
        return res;
      }

      m[nums[i]] = i;
    }
    return res;
  }
};
}  // namespace lc1

TEST(twoSum, sol1) {
  lc1::Solution s;
  auto PrintRes = [](vector<int>& r) {
    if (r.size() == 2) {
      log_info("res: {}, {}", r[0], r[1]);
    }
  };
  vector<int> nums1 = {1, 2, 3};
  auto r1 = s.twoSum(nums1, 5);
  PrintRes(r1);

  vector<int> nums2 = {3, 2, 4};
  auto r2 = s.twoSum(nums2, 6);
  PrintRes(r2);

  vector<int> nums3 = {-10, 7, 19, 15};
  auto r3 = s.twoSum(nums3, 9);
  PrintRes(r3);
}

/*
namespace lc2 {

struct ListNode {
  int val;
  ListNode* next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode* next) : val(x), next(next) {}
};

class Solution {
 public:
  ListNode Arr[105];
  ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    memset(Arr, 0, sizeof(Arr));

    ListNode *p1 = l1, p2 = l2, newadd, last = nullptr, tailp = nullptr,
             head = nullptr;
    int idx = 0;

    while (p1 != nullptr && p2 != nullptr) {
      newadd = &(Arr[idx++]);
      // alloc

      newadd->val = p1->val + p2->val;
      if (last != nullptr) {
        last->next = newadd;
      }
      last = newadd;
      p1 = p1->next;
      p2 = p2->next;
    }  // common part

    tailp = p1 != nullptr ? p1 : p2;
    while (last != nullptr && tailp != nullptr) {
      newadd = &(Arr[idx++]);
      newadd->val = tailp->val;
      last->next = newadd;
      tailp = tailp->next;
    }

    if (idx != 0) {
      head = &(Arr[0]);
    }
    while (head != nullptr) {
      if (head->val > 10) {
        int addon = head->val / 10;
        if (head->next != nullptr) {
          head->next->val += addon;
        } else {
          newadd = &(Arr[idx++]);
          newadd->val = addon;
          head->next = newadd;
        }
        head->val = head->val % 10;
      }
      head = head->next;
    }
    return &(Arr[0]);
  }
};
}  // namespace lc2
*/

#include <list>
#include <unordered_map>
namespace lc146 {
class LRUCache {
 public:
  LRUCache(int capacity) : capa_(capacity) {
    hash_.clear();
    lrulist_.clear();
  }

  std::list<int>::iterator move_front(std::list<int>::iterator pos) {
    int k = *pos;
    lrulist_.erase(pos);
    lrulist_.emplace_front(k);
    return lrulist_.begin();
  }

  int get(int key) {
    auto r = hash_.find(key);
    if (r == hash_.end()) {
      return -1;
    }
    r->second.pos_ = move_front(r->second.pos_);
    return r->second.val_;
  }

  void put(int key, int value) {
    auto r = hash_.find(key);
    std::list<int>::iterator pos;
    if (r == hash_.end()) {
      lrulist_.emplace_front(key);
      pos = lrulist_.begin();
      if (lrulist_.size() > capa_) {
        int elimkey = lrulist_.back();
        hash_.erase(elimkey);
        lrulist_.pop_back();
      }
    } else {
      pos = move_front(r->second.pos_);
    }
    hash_[key].val_ = value;
    hash_[key].pos_ = pos;
  }

  int capa_;
  struct mapped_entry {
    int val_;
    std::list<int>::iterator pos_;
    mapped_entry() {}
    mapped_entry(int v, std::list<int>::iterator p) : val_(v), pos_(p) {}
  };
  std::unordered_map<int, mapped_entry> hash_;
  std::list<int> lrulist_;
};
}  // namespace lc146

TEST(lc146, sol_c1) {
  lc146::LRUCache L(2);
  L.put(1, 1);
  L.put(2, 2);
  log_info("{}", L.get(1));
  L.put(3, 3);
  log_info("{}", L.get(2));
  L.put(4, 4);
  log_info("{}", L.get(1));
  log_info("{}", L.get(3));
  log_info("{}", L.get(4));
}
TEST(lc146, sol_c2) {
  lc146::LRUCache L(2);
  log_info("{}", L.get(2));
  L.put(2, 6);
  log_info("{}", L.get(1));
  L.put(1, 5);
  L.put(1, 2);
  log_info("{}", L.get(1));
  log_info("{}", L.get(2));
}

#include <cmath>

namespace lc17_06 {
class Solution {
 public:
  uint64_t countDigitInKSlot(uint64_t n, uint64_t k, uint64_t limit,
                             const std::map<uint64_t, uint64_t>& b) {
    uint64_t d = b.at(k);
    uint64_t power = uint64_t(pow(10, k));
    uint64_t less2 = n / (power * 10) * power;
    if (d < 2) {
      return less2;
    } else if (d > 2) {
      return (n / (power * 10) + 1) * power;
    } else {
      return less2 + (n + 1 - n / uint64_t(pow(10, k)) * uint64_t(pow(10, k)));
    }
  }

  uint64_t numberOf2sInRange(uint64_t n) {
    std::map<uint64_t, uint64_t> n2i;
    uint64_t t = n, c = 0, cmax;
    while (t > 0) {
      n2i[c] = t % 10;
      t = t / 10;
      c = c + 1;
    }
    cmax = c;
    uint64_t sum = 0;
    for (uint64_t i = 0; i < cmax; i++) {
      sum += countDigitInKSlot(n, i, cmax, n2i);
    }
    return sum;
  }
};
}  // namespace lc17_06

TEST(lc17_06, sol) {
  lc17_06::Solution s1;
  log_info("{}", s1.numberOf2sInRange(5));
  log_info("{}", s1.numberOf2sInRange(100));
}

// a prototype of skiplist
//
#include <random>
namespace lc1206 {
class Skiplist {
  using KeyType = int;
  struct Node {
    bool k_is_valid_;
    KeyType k_;
    std::vector<Node*> next_;
    Node() : k_is_valid_(false) {}
    Node(KeyType k, int lvl) : k_is_valid_(true), k_(k) {
      next_.resize(lvl, nullptr);
    }
    void set_next(int idx, Node* p) {
      int maxidx = next_.size() - 1;
      if (idx > maxidx) {
        int add = idx - maxidx;
        for (int i = 0; i < add; i++) {
          next_.emplace_back(nullptr);
        }
      }
      next_[idx] = p;
    }
  };
  int random100() { return dist_(generator_); }
  int get_level() {
    int l = 1;
    while (random100() < P_ && l <= MaxLvl_) {
      l++;
    }
    return l;
  }

  std::default_random_engine generator_;
  std::uniform_int_distribution<int> dist_;
  int P_;
  int MaxLvl_;  // max insert 50000 times
  Node head_;
  std::list<Node*> alloc_track_;

 public:
  Skiplist() : dist_(1, 100), P_(25), MaxLvl_(17) {}
  ~Skiplist() {
    for (auto& i : alloc_track_) {
      if (i != nullptr) {
        delete i;
      }
    }
  }
  int curr_lvl() { return head_.next_.size(); }

  Node* internal_search(KeyType target, std::vector<Node*>& preceeds) {
    preceeds.clear();
    int currlvl = curr_lvl();
    for (int i = 0; i < currlvl; i++) {
      preceeds.emplace_back(&head_);
    }
    Node* pcurr = &head_;
    int plvl = pcurr->next_.size() - 1;
    Node* retitem = nullptr;
    while (plvl >= 0 && pcurr != nullptr) {
      if (pcurr->k_is_valid_ && pcurr->k_ == target) {
        retitem = pcurr;
        for (int i = plvl; i >= 0; i--) {
          preceeds[i] = pcurr;
        }
        break;
      } else {
        Node* c1 = nullptr;
        plvl++;
        do {
          plvl--;
          if (plvl >= 0) {
            c1 = pcurr->next_[plvl];
          }
        } while (c1 != nullptr && c1->k_is_valid_ && target < c1->k_ &&
                 plvl >= 0);
        if (plvl < 0) {
          preceeds[0] = pcurr;
        } else {
          preceeds[plvl] = c1;
          pcurr = c1;
        }
      }  // ...
    }
    return retitem;
  }

  bool search(int target) {
    std::vector<Node*> pp;
    Node* r = internal_search(target, pp);
    return r != nullptr;
  }

  void add(int num) {
    std::vector<Node*> pp;
    Node* r = internal_search(num, pp);

    int lvl = get_level();

    Node* newadd = new Node(num, lvl);
    alloc_track_.emplace_back(newadd);

    int min = lvl <= pp.size() ? lvl : pp.size();
    for (int i = 0; i < min; i++) {
      newadd->next_[i] = pp[i]->next_[i];
      pp[i]->set_next(i, newadd);
    }
    if (lvl > pp.size()) {
      for (int i = pp.size(); i < lvl; i++) {
        head_.set_next(i, newadd);
      }
    }
  }

  bool erase(int num) {
    std::vector<Node*> pp;
    Node* r = internal_search(num, pp);
    if (r != nullptr) {
      int rlvl = r->next_.size() - 1;
      for (int i = rlvl; i >= 0; i--) {
        pp[i]->next_[i] = r->next_[i];
      }
      return true;
    }
    return false;
  }
};
}  // namespace lc1206

TEST(lc1206, sol) {
  lc1206::Skiplist l;
  l.add(4);
  l.add(6);
  log_info("{}", l.search(4));
  log_info("{}", l.search(0));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module();
  return RUN_ALL_TESTS();
}
