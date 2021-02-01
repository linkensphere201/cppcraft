// keywords: shared_ptr
//

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <functional>
#include <malloc.h>
#include <thread>
#include <mutex> 
#include <chrono>
#include <random>
#include "log.h"

void ShowTopic() {
  log_info("Topic: " "shared_ptr");
}

TEST(functor, ShowTopic) {
  ShowTopic();
}

class CDeleter {
  public:
    void operator()(void *x) {
      free(reinterpret_cast<char*>(x));
    }
};

class CppDeleter {
  public:
    void operator()(void *x) {
      delete reinterpret_cast<char*>(x);
    }
};

/*
 * 
 * undefined bebavior:
 * 1. what if call free() to an object which is created by new
 * 1. what if call delete to an object which is created by malloc
 * https://stackoverflow.com/questions/10854210/behaviour-of-malloc-with-delete-in-c
 * https://isocpp.org/wiki/faq/freestore-mgmt
 * */

TEST(sharedptr, custom_deleter) {
  void *p;
  p = malloc(100);
  
  //
  // custom_deleter: 
  //  1. should be copy-constructible
  //  2. shoule take a parameter of T* where T is the type of pointer
  //  3. can be a functor/function/...
  std::shared_ptr<void> x1(p, CDeleter());

  p = malloc(100);
  std::shared_ptr<void> x2(p, [](void*p){free(p);});
  
  // some trick
  // order of destruction of stack variable: https://stackoverflow.com/questions/14688285/c-local-variable-destruction-order
  std::shared_ptr<void> a1(nullptr, [](void*){
        log_info("this is a defer...");
      });
  
  std::shared_ptr<void> a2(nullptr, [](void*){
        log_info("this is another defer...");
      });
}

/* 
 * 可调用对象
 *  https://zhuanlan.zhihu.com/p/110591071
 *  https://stackoverflow.com/questions/19278424/what-is-a-callable-object-in-c
 *  https://blog.csdn.net/wangshubo1989/article/details/49134235
 * std::function:
 *   1. 统一了functor/function pointer/lambda
 *   2. 
 * */
class Del {
  public:
    void operator()(char* x) {
      log_info("calling Del::opeartor()");
    }
};

void funcDel(char *) {
  log_info("calling func Del");
}

TEST(sharedptr, callable) {
  using optype = std::function<void(char*p)>;
  optype obj1 = Del(),
         obj2 = funcDel;
  optype obj3 = [](char *){ 
      log_info("calling lambda");
    };

  char *tmp;
  obj1(tmp);
  obj2(tmp);
  obj3(tmp);
}


TEST(sharedptr, weakptr) {

}

class App {
public:
  App() : id_(App::global_id_++) {
    log_info("#{} of App is created!", id_);
  }
  ~App() {
    log_info("#{} of App is destoried!", id_);
  }
  int getid() {
    return id_;
  }
private: 
  int id_;
  static int global_id_;
};
int App::global_id_ = 0;
using AppPtr = std::shared_ptr<App>;

void do_something(App *a) {
  log_info("do #{} ...", (a != nullptr ? a->getid() : -1));
}
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1,6);
void reader(AppPtr& ap, std::mutex &apmtx) {
  log_info("a and a_mtx: {},{}",(void*)(&ap), (void*)(&apmtx));
  std::this_thread::sleep_for(std::chrono::milliseconds(distribution(generator)));
  AppPtr local;
  {
    log_info(">>> enter critical");
    std::lock_guard<std::mutex> l(apmtx);
    local = ap;
    log_info("<<< leave critical");
  }
  do_something(local.get());
}
void writer(AppPtr& ap, std::mutex &apmtx) {
  log_info("a and a_mtx: {},{}",(void*)(&ap), (void*)(&apmtx));
  std::this_thread::sleep_for(std::chrono::milliseconds(distribution(generator)));
  AppPtr local_old,local_new(new App);
  {
    log_info(">>> enter critical");
    std::lock_guard<std::mutex> l(apmtx); 
    local_old = ap;
    ap = local_new;
    log_info("<<< leave critical");
  }
}
// please check: 
//  1. https://www.boost.org/doc/libs/1_75_0/libs/smart_ptr/doc/html/smart_ptr.html#shared_ptr_thread_safety
//  2. linux服务端多线程编程（muduo）: Section 1.9
TEST(sharedptr,threadsafety) {
  AppPtr a = std::make_shared<App>(); 
  std::mutex a_mtx;
  std::thread t1(reader, std::ref(a), std::ref(a_mtx));
  std::thread t2(writer, std::ref(a), std::ref(a_mtx));
  t1.join();
  t2.join();
}

void Use_make_shared() {
  // check: 
  //  1. https://stackoverflow.com/questions/20895648/difference-in-make-shared-and-normal-shared-ptr-in-c
  //  2. cn: https://www.cnblogs.com/leijiangtao/p/12046333.html
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module(); 
  return RUN_ALL_TESTS();
}
