#pragma once

#include <fmt/core.h>
#include <functional>
#include <list>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "log.h"

namespace cp1craft {
namespace utils {

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

//
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

  using HookOnRemove = std::function<void(Key &)>;
  bool Remove(const Key &k, HookOnRemove f);

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
  int node_count_;
  // current level: [0, curr_lvl_]
};

template<typename Key>
Skiplist<Key>::Skiplist(int seed, int policy)
    : generator_(seed), dist_(1, kPMax_), cmp_(GetDefComparator<Key>()),
      plc_(policy), head_(nullptr), curr_lvl_(0), node_count_(0) {
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
bool Skiplist<Key>::Remove(const Key &k, Skiplist<Key>::HookOnRemove f) {
  std::vector<Node *> preceeds;
  findPreceeds(k, preceeds);
  auto may_eq = preceeds[0]->GetNext(0);
  if (may_eq != nullptr && may_eq->Get() == k) {
    int ed = may_eq->GetLevel();
    for (int i = 0; i <= ed; i++) {
      if (may_eq == preceeds[i]->GetNext(i)) {
        preceeds[i]->SetNext(i, may_eq->GetNext(i));
      }
    }
    if (f != nullptr)
      f(may_eq->k_);
    return true;
  }
  return false;
}

template<typename Key>
bool Skiplist<Key>::Search(Key &k, Skiplist<Key>::HookOnFound f) {
  std::vector<Node *> preceeds;
  findPreceeds(k, preceeds);
  auto may_eq = preceeds[0]->GetNext(0);
  if (may_eq != nullptr && may_eq->Get() == k) {
    if (f != nullptr)
      f(may_eq->Get(), k); // call user hook of hit
    return true;
  }
  return false;
}

template<typename Key>
void Skiplist<Key>::DumpAsGraphvizScatch(bool node_full_ctn, std::string &r) {
  std::stringstream ss;
  std::string header = "----dump as graphviz---\n\
digraph {  \n\
  rankdir=LR \n\
  node [shape=record] \n\
  nodesep=0",
              tailer = "}", nprefix = "N", lprefix = "l";
  ss << header << "\n";
  // output node def
  auto get_node_idstr = [this, nprefix, lprefix](Node *p, bool defname,
                                                 bool full) -> std::string {
    std::string nid = nprefix + std::to_string(p->GetId());
    if (defname) {
      nid += "[label=\"";
      bool is_first = true;
      for (int i = p->next_.size() - 1; i >= 0; i--) {
        if (i <= curr_lvl_) {
          std::string label = fmt::format("<{}{}>", lprefix, i);
          label += i == 0 ? (p != head_ ? p->ToString(full) : "HEAD") : "*";

          nid += (is_first ? label : (std::string("|") + label));
          if (is_first) {
            is_first = false;
          }
        }
      }
      nid += "\"]";
    }
    return nid;
  };

  // output node definitions
  Node *i = head_;
  while (i != nullptr) {
    ss << get_node_idstr(i, true, node_full_ctn) << "\n";
    i = i->GetNext(0);
  }

  // output node label relations
  for (int lvl = curr_lvl_; lvl >= 0; lvl--) {
    Node *itr = head_;
    bool is_first = true;
    while (itr != nullptr) {
      std::string srclabel = fmt::format(
          "{}:{}{}", get_node_idstr(itr, false, node_full_ctn), lprefix, lvl);
      itr = itr->GetNext(lvl);
      if (itr != nullptr) {
        std::string dstlabel = fmt::format(
            "{}:{}{}", get_node_idstr(itr, false, node_full_ctn), lprefix, lvl);
        ss << srclabel << "->" << dstlabel << "\n";
      }
    }
  }
  ss << tailer << "\n";
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
  Node *n = new Node(level, k, node_count_++);
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
  Node(int lvl, Key k, int nid) : k_(k), lvl_(lvl), id_(nid) {
    next_.resize(lvl_ + 1, nullptr);
  }
  int GetId() { return id_; }
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
    return full ? fmt::format("{}-{:p}", k_, (void *)this)
                : fmt::format("{}", k_);
  }

private:
  friend class Skiplist<Key>;
  Key k_;
  int lvl_;
  int id_;
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

} // namespace utils
} // namespace cp1craft


// a formatter for SimpleKeyHandle
template<>
struct fmt::formatter<cp1craft::utils::SimpleKeyHandle> : fmt::formatter<std::string> {
  template<typename FormatContext>
  auto format(const cp1craft::utils::SimpleKeyHandle &skh, FormatContext &ctx)
      -> decltype(ctx.out()) {
    return fmt::formatter<std::string>::format(skh.ToString(false), ctx);
  }
};

