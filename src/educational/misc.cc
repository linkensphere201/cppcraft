// keywords: misc
//  - keyword default/delete
//    https://www.geeksforgeeks.org/explicitly-defaulted-deleted-functions-c-11/
//  - keyword virtual/override/final
//    https://stackoverflow.com/questions/13880205/is-the-override-keyword-just-a-check-for-a-overridden-virtual-method
//    https://www.fluentcpp.com/2020/02/21/virtual-final-and-override-in-cpp/
//   virtual

#include <gtest/gtest.h>
#include <iostream>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "misc");
}

TEST(misc, ShowTopic) {
  ShowTopic();
}

/* TOPIC1 - override
 *  `override` basic usage:
 *    - mark override in a derived function
 *  
 * */

class Base {
public:
  virtual void dosome();
};

class Xderived : public Base {
public:
  void dosome() override; // note that this function is override some virtual member function base class
  // void dosome() const override; 
    // compile failed: error: ‘void Xderived::dosome() const’ marked ‘override’, but does not override
    // because of no function found in base class
};
void Base::dosome() {
  log_info("default func!");
}

void Xderived::dosome() {
  log_info("derived func!");
}

TEST(misc, keyword_override) {
}

/*
 * TOPIC2 - constexpr
 *  static const vs. static constexpr
 *
 * */
struct T1 {
  static const int a;
  static const int b = 100;
  // static constexpr int c; // ‘constexpr’ static data member ‘c’ must have an initializer
  static constexpr int d = 101;
};
const int T1::a = 10;

TEST(misc, keyword_constexpr) {
  log_info("{}", T1::a);
}

/*
 * TOPIC3 - substract class
 * a class with pure virtual function can not have an instance.
 * a class whose base class have a pv function and it do not impl that pv function, then this class is also a substract class(so no instance is allowed).
 * a class with p-v function can have a concrete function as `default` impl.
 * */
class T2 {
  public:
    virtual void do_some() = 0;
    virtual void do_other();
};

void T2::do_some() {
  log_info("default do_some");
}
void T2::do_other() {
  log_info("default do_other");
}

class a : public T2 {
  public:
    virtual void do_some() override;
    //virtual void do_other() override;
};

void a::do_some() {
  log_info("a-impl do_some");
}
/*
void a::do_other() {
  log_info("a-impl do_other");
}
*/
TEST(misc, virtual_) {
  // T2 x; compile error:
  a x1;
  x1.do_some();
  x1.do_other();
}

/*
 * topic - nested template class: how to design
 *  refer to: 
 *   - src/educational/skiplist.hpp
 *   - https://stackoverflow.com/questions/10250023/how-to-design-nested-template-classes-where-nested-classes-are-of-the-same-type
 * */

template<typename T1>
class A {
public:
  A(T1 a) : b_(nullptr) {
    b_ = new B(a);
  } 
  ~A() {
    if (b_ != nullptr) {
      delete b_;
    }
  }
  class B;
  B* b_;
};

template<typename T1>
class A<T1>::B {
public:
  B(T1 a) : in_(a) {}
  T1 get() { return in_; }
  T1 in_;
};

TEST(misc, nested_template_class ) {
  A<int> a(100);
  log_info("test: {}", a.b_->get());
}




int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module(); 
  return RUN_ALL_TESTS();
}
