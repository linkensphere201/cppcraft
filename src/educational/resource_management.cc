
// keywords:
// std::shared_ptr
// std::weak_ptr
// std::make_shared
// std::enable_shared_from_this
// std::bind
//
//

#include <gtest/gtest.h>
#include <memory>
#include <iostream>


void ThreadSafety_of_shared_ptr() {
  // please check: 
  //  1. https://www.boost.org/doc/libs/1_75_0/libs/smart_ptr/doc/html/smart_ptr.html#shared_ptr_thread_safety
  //  2. linux服务端多线程编程（muduo）: Section 1.9
  auto GetOrSetPtr = [] () {
  };
}

void Use_make_shared() {
  // check: 
  //  1. https://stackoverflow.com/questions/20895648/difference-in-make-shared-and-normal-shared-ptr-in-c
  //  2. cn: https://www.cnblogs.com/leijiangtao/p/12046333.html
}

void Use_weak_ptr() {
  // check:
  //  1. scenario of weak_ptr: https://www.jianshu.com/p/eee31203d05f
  //
  std::shared_ptr<int> x();
}


void Use_enable_shared_from_this () {
  // check: 
  //  1. https://blog.csdn.net/ws891033655/article/details/40558837
  //  2. check the impl of boost
}

TEST(resmgr, UseWeakPtr) { UseWeakPtr(); }

class X {
 public:
  X(int id) : id_(id) {}
  ~X() { std::cout << "X[" << id_ << "] is destoried." << std::endl; }
  void GetSharedPtrFromWithinMemberFuncV0() {
    std::shared_ptr<X> fuck(this);
    /* any attempt to construct a `shared_ptr`
        to manage `this` is EXTREMELY DANGEROUS:
        this object `fuck` would call `~X()` because it think it is
        the last one who holds the resource, so the program would crash and generate a coredump.
        */
  }
 private:
  int id_;
};

void BadExample1() {
  std::shared_ptr<X> external_p(new X(101));
  external_p->GetSharedPtrFromWithinMemberFuncV0();
}

TEST(resmgr, BadExample1) { BadExample1(); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
