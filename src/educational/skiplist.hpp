#pragma once

#include <fmt/core.h>
#include <functional>
#include <list>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "log.h"

template<typename Key>
class Comparator {
public:
  Comparator(const std::string &name) : name_(name) {}
  std::string GetName() { return name_; }
  virtual int operator()(const Key &a, const Key &b) = 0;

private:
  std::string name_;
};

template<typename Key>
class DefComparator : public Comparator<Key> {
public:
  DefComparator() : Comparator<Key>("default_comparator") {}
  virtual int operator()(const Key &a, const Key &b) {
    if (std::less<Key>()(a, b)) {
      return -1;
    } else if (std::greater<Key>()(a, b)) {
      return 1;
    } else {
      return 0;
    }
  }
};

// a template function
template<typename Key>
DefComparator<Key> *GetDefComparator() {
  static DefComparator<Key> gCmp;
  return &gCmp;
}

class SimpleKeyHandle {
public:
  SimpleKeyHandle() : key_(777) {}
  explicit SimpleKeyHandle(int k) : key_(k) {}
  SimpleKeyHandle(int k, void *v) : key_(k), val_(v) {}
  bool operator<(const SimpleKeyHandle &rhs) const { return key_ < rhs.key_; }
  bool operator>(const SimpleKeyHandle &rhs) const { return key_ > rhs.key_; }
  bool operator==(const SimpleKeyHandle &rhs) const { return key_ == rhs.key_; }
  std::string ToString(bool full) const {
    return full ? fmt::format("{} -> {}", key_, val_) : fmt::format("{}", key_);
  }

private:
  int key_;
  void *val_;
};

// a formatter for SimpleKeyHandle
template<>
struct fmt::formatter<SimpleKeyHandle> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const SimpleKeyHandle &skh, FormatContext &ctx)
      -> decltype(ctx.out()) {
    return fmt::formatter<std::string>::format(skh.ToString(false), ctx);
  }
};

//
// TODO - `Key` should be designed as a handle
//
template<typename Key>
class Skiplist {
public:
  class Node;
  struct Policy {
    static constexpr int kKeyAllowedMulti = 1 << 0;
    Policy() : p_(0) {}
    Policy(int p) : p_(p) {}
    bool IsAllowMulti() { return p_ & kKeyAllowedMulti; }
    int p_;
  };
  Skiplist(int seed = 0, int policy = 0);
  ~Skiplist();

  bool Insert(const Key &k);
  bool Remove(const Key &k);

  using HookOnFound = std::function<void(const Key &, Key &)>;
  bool Search(Key &k, HookOnFound f);

  void DumpAsGraphvizScatch(bool node_full_id, std::string &r);

  int GetLevel();
  Node *newNode(Key k, int level);
  void findPreceeds(Key k, std::vector<Node *> &r);

public:
  static constexpr int kP_ = 50;
  static constexpr int kPMax_ = 100;
  static constexpr int kMaxLvl_ = 12;

private:
  std::mt19937 generator_;
  std::uniform_int_distribution<int> dist_;

  Comparator<Key> *cmp_;
  Policy plc_;

  Node *head_;
  std::list<Node *> alloc_track_;
  int curr_lvl_;
  // current level: [0, curr_lvl_]
};

template<typename Key>
Skiplist<Key>::Skiplist(int seed, int policy)
    : generator_(seed), dist_(1, kPMax_), cmp_(GetDefComparator<Key>()),
      plc_(policy), head_(nullptr), curr_lvl_(0) {
  head_ = newNode(Key{}, kMaxLvl_); // a default value of Key
}

template<typename Key>
Skiplist<Key>::~Skiplist() {
  for (auto &i : alloc_track_) {
    if (i != nullptr) {
      delete i;
    }
  }
}

template<typename Key>
bool Skiplist<Key>::Insert(const Key &k) {
  // TODO - fast path for sequantial insert
  std::vector<Node *> preceeds;
  findPreceeds(k, preceeds);
  assert(preceeds[0] != nullptr);
  auto may_eq = preceeds[0]->GetNext(0);
  if (may_eq != nullptr && may_eq->Get() == k && !plc_.IsAllowMulti()) {
    return false;
  }

  int nlvl = GetLevel();
  auto nnode = newNode(k, nlvl);

  int ph1_ed = nlvl <= curr_lvl_ ? nlvl : curr_lvl_;
  for (int i = ph1_ed; i >= 0; i--) {
    assert(preceeds[i] != nullptr);
    assert(preceeds[i]->GetLevel() >= i);
    Node *nxt = preceeds[i]->GetNext(i);
    nnode->SetNext(i, nxt);
    preceeds[i]->SetNext(i, nnode);
  }
  if (nlvl > curr_lvl_) {
    int ph2_st = curr_lvl_ + 1, ph2_ed = nlvl;
    for (int i = ph2_st; i <= ph2_ed; i++) {
      head_->SetNext(i, nnode);
    }
    curr_lvl_ = nlvl;
  }
  return true;
}

template<typename Key>
bool Skiplist<Key>::Remove(const Key &k) {
  return false;
}

template<typename Key>
bool Skiplist<Key>::Search(Key &k, Skiplist<Key>::HookOnFound f) {
  return false;
}

template<typename Key>
void Skiplist<Key>::DumpAsGraphvizScatch(bool node_full_id, std::string &r) {
  std::stringstream ss;
  for (int lvl = curr_lvl_; lvl >= 0; lvl--) {
    Node *itr = head_;
    bool is_first = true;
    while (itr != nullptr) {
      std::string s = itr->ToString(node_full_id);

      if (is_first) {
        is_first = false;
      } else {
        ss << "->";
      }
      ss << s;

      itr = itr->GetNext(lvl);
    }
    ss << "\n";
  }
  r = ss.str();
}

template<typename Key>
int Skiplist<Key>::GetLevel() {
  int l = 0;
  while (dist_(generator_) < kP_ && l <= kMaxLvl_) {
    l++;
  }
  return l;
}

template<typename Key>
typename Skiplist<Key>::Node *Skiplist<Key>::newNode(Key k, int level) {
  Node *n = new Node(level, k);
  alloc_track_.emplace_back(n);
  log_info("new add: k={}, lvl={}", n->Get(), n->GetLevel());
  return n;
}

template<typename Key>
void Skiplist<Key>::findPreceeds(Key k, std::vector<Node *> &r) {
  r.resize(curr_lvl_ + 1, head_);

  int lvl = curr_lvl_;
  Node *node = head_;
  while (lvl >= 0 && node != nullptr) {
    Node *nxt = node->GetNext(lvl);

    // n      ln
    // *   *  *
    //     k
    //
    if (nxt != nullptr && (*cmp_)(k, nxt->Get()) > 0) {
      node = nxt;
    } else { // nxt == nullptr
             // k < nxt->Get()
      r[lvl] = node;
      lvl--;
    }
  }
}

//
// "lvl_" means this node's topest level.
// thus this node have an array of {"lvl_" + 1} items.
//
template<typename Key>
class Skiplist<Key>::Node {
public:
  Node(int lvl, Key k) : k_(k), lvl_(lvl) { next_.resize(lvl_ + 1, nullptr); }
  Key Get() { return k_; }
  int GetLevel() { return lvl_; }
  // lvl: [0,lvl_ - 1]
  bool SetNext(int lvl, Node *n) {
    // assert(lvl >= 0 && lvl <= next_.size() - 1);
    if (lvl >= 0 && lvl <= next_.size() - 1) {
      next_[lvl] = n;
      return true;
    }
    log_error("fatal: setnext: {}, max: {}", lvl, next_.size() - 1);
    return false;
  }
  Node *GetNext(int lvl) {
    if (lvl >= 0 && lvl <= next_.size() - 1) {
      return next_[lvl];
    }
    log_error("fatal: getnext: {}, max: {}", lvl, next_.size() - 1);
    return nullptr;
  }
  std::string ToString(bool full = false) {
    return full ? fmt::format("N{}-{:p}", k_, (void *)this)
                : fmt::format("N{}", k_);
  }

private:
  Key k_;
  int lvl_;
  std::vector<Node *> next_;
  // I'm prefer to use a vector than a struct hack
  // which may look like below:
  //    ```
  //    struct x {
  //       ...
  //       SomeType node [0];
  //    };
  //    ```
  //
};

