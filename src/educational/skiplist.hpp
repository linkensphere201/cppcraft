#pragma once

#include <functional>
#include <list>
#include <random>
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

template<typename Key>
class Skiplist {
  class Node;

public:
  Skiplist(int seed = 0);
  ~Skiplist();
  int GetLevel();

private:
  Node* newNode(Key k, int level);
  void findPreceeds(Key k, std::vector<Node*> &r);

public:
  static constexpr int kP_ = 25;
  static constexpr int kPMax_ = 100;
  static constexpr int kMaxLvl_ = 12;

private:
  std::mt19937 generator_;
  std::uniform_int_distribution<int> dist_;

  Node *head_;
  std::list<Node *> alloc_track_;
  Comparator<Key> *cmp_;

  int curr_lvl_; 
  // current level: [0, curr_lvl_]
};

template<typename Key>
Skiplist<Key>::Skiplist(int seed)
    : generator_(seed), dist_(1, kPMax_), head_(nullptr), cmp_(GetDefComparator<Key>()), curr_lvl_(0) {
  head_ = newNode(0, kMaxLvl_);
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
int Skiplist<Key>::GetLevel() {
  int l = 0;
  while (dist_(generator_) < kP_ && l <= kMaxLvl_) {
    l++;
  }
  return l;
}

template<typename Key>
typename Skiplist<Key>::Node* Skiplist<Key>::newNode(Key k, int level) {
  Node *n = new Node(level, k);
  alloc_track_.emplace_back(n);
  return n;
}

template<typename Key>
void Skiplist<Key>::findPreceeds(Key k, std::vector<Node*> &r) {
  r.resize(curr_lvl_ + 1);
  for (int i = 0; i < r.size(); i++) {
    r[i] = head_;
  }

  int l = curr_lvl_;
  Node* n = head_;
  while (l >= 0) {
    if (n != nullptr) {
      Node* ln = n->GetNext(l);
      if (ln != nullptr && (*cmp_)(ln->Get(), k) < 0) {}
    }
  }
}

//
// a "lvl_" node means this node have {"lvl_" + 1} next array.
//
template<typename Key>
class Skiplist<Key>::Node {
public:
  Node(int lvl, Key k) : k_(k), lvl_(lvl) { next_.resize(lvl_ + 1, nullptr); }
  Key Get() { return k_; }
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
  Node* GetNext(int lvl) {
    if (lvl >= 0 && lvl <= next_.size() - 1) {
      return next_[lvl];
    }
    log_error("fatal: getnext: {}, max: {}", lvl, next_.size() - 1);
    return nullptr;
  }

private:
  Key k_;
  int lvl_;
  std::vector<Node *> next_;
};

