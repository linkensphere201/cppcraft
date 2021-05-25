#include <memory>
#include <thread>
#include <folly/MPMCQueue.h>
#include <folly/futures/Future.h>
#include <folly/portability/GTest.h>

#include "log.h"
#include "utils.h"

using namespace folly;

TEST(Future, isReady) {
  Promise<int> p;
  auto f = p.getFuture();
  EXPECT_FALSE(f.isReady());
  p.setValue(42);
  EXPECT_TRUE(f.isReady());
}

struct ctx {
  int a;
  int b;
  Promise<int> res;
  ctx() noexcept {}
  ctx(int a_, int b_, Promise<int> && p) noexcept : a(a_), b(b_), res(std::move(p)) {}
  ctx(ctx&& c) noexcept : a(c.a), b(c.b), res(std::move(c.res)) {}
};

void operate(ctx &c) {
  c.res.setValue(c.a + c.b);
}

void Reactor(folly::MPMCQueue<std::shared_ptr<ctx>> &q) {
  while(1) {
    std::shared_ptr<ctx> c1;
    if (q.read(c1)) {
      log_info("operate 1 ctx...");
      std::this_thread::sleep_for(std::chrono::seconds(3));
      operate(*c1);
      if (c1->a == 0 && c1->b == 0) {
        break;
      }
    } else {
      log_info("reactor idle...");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

folly::Future<int> Submit(int a, int b, folly::MPMCQueue<std::shared_ptr<ctx>> &q) {
  Promise<int> p;
  auto r = p.getFuture();
  auto c = std::make_shared<ctx>(a, b, std::move(p));
  if (q.write(c)) {
    log_info("submit {},{} ok...", a, b);
  }
  // not consider fail....
  return r;
}

TEST(Future, threaded) {
  folly::MPMCQueue<std::shared_ptr<ctx>> cq(100);
  std::thread(Reactor, std::ref(cq)).detach();
  
  log_info("wait 5 secs...");
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  auto r = Submit(1, 2, cq);
  while(1) {
    if (r.isReady()) {
      log_info("done: {}", r.value());
      Submit(0, 0, cq); // tell reactor to turn off
      break;
    } else {
      log_info("not ready...");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
