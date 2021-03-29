// keywords: DelReverseN
//

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "DelReverseN");
}

TEST(DelReverseN, ShowTopic) {
  ShowTopic();
}

struct node {
  int val;
  node *next;
  node (int v) : val(v), next(nullptr){}
  ~node() {
    log_info("del {}", val);
  }
};

node* ConstructList(int length) {
  node *prev = nullptr;
  node *head = nullptr;

  for (int i = 1; i <= length; i++) {
    node *alloc = new node(i);
    if (head == nullptr) {
      head = alloc;
    }
    if (prev != nullptr) {
      prev->next = alloc;
    }
    prev = alloc;
  }
  return head;
}

void printer(node *l, std::function<void(node*)> op) {
  while(l != nullptr) {
    op(l);
    l = l->next;
  }
}

node* doReverse(node* head) {
  node *curr, *next, *rethead;
  node *tmp = nullptr;
  curr = head;
  next = curr ? curr->next : nullptr;
  while(curr != nullptr) {
    if (next != nullptr) {
      tmp = next->next;
      // log_info("op: {} <- {}", curr->val, next->val);
      next->next = curr;
    } else {
      // log_info("sh: {}", curr->val);
      rethead = curr;
    }
    curr = next;
    next = tmp;
  }
  head->next = nullptr;
  return rethead;
}

node* doDelN(node *head, int n) {
  node *curr, *prev;
  node *rethead = head;
  curr = head;
  prev = nullptr;
  int idx = 1;
  while(true) {
    if (idx == n) {
       if (prev != nullptr) {
          prev->next = curr->next;
        } else {
          rethead = curr->next;
        }
        break;    
    }
    prev = curr;
    curr = curr->next;
    idx++;
  }
  return rethead;
}


TEST(DelReverseN, constor) {
  node *head = ConstructList(10);
  node *nl = doReverse(head);
  printer(nl, [](node *a){
        log_info("{}", a->val);
      });
}

TEST(DelReverseN, deln) {
  node *head = ConstructList(10);
  node *nl = doDelN(head, 2);
  printer(nl, [](node *a){
        log_info("{}", a->val);
      });
}

TEST(DelReverseN, delrn) {
  node *head = ConstructList(10);
  node *rl = doReverse(head);
  node *dl = doDelN(rl, 2);
  node *ol = doReverse(dl);
  printer(ol, [](node *a){
        log_info("{}", a->val);
      });
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
